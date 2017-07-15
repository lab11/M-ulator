/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2016  Pat Pannuto <pat.pannuto@gmail.com>
 *
 * This file is part of Mulator.
 *
 * Mulator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mulator.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/stat.h>

// **HACK** Sigh.. platform inconsistency, this needs something autotools-y
//          to figure out if it's <libelf.h> or <libelf/libelf.h>, so for this
//          moment, it's just turned off as nothing will ever trip this define
#ifdef __HAVE_LIBELF__

// ELF code relies heavily on `libelf by Example` by Joseph Koshy
#include <libelf/libelf.h>
#include <libelf/gelf.h>
#endif // __HAVE_LIBELF__

// Do this define manually b/c it's useful / common
#define PT_ARM_EXIDX (PT_LOPROC + 1) // ARM unwind segment.


#include "loader.h"

#include "core/simulator.h"
#include "core/state_sync.h"

#include "cpu/core.h"
#include "cpu/common/rom.h"
#include "cpu/common/ram.h"

EXPORT void flash_image(const uint8_t *image, const uint32_t num_bytes) {
#if defined (HAVE_ROM)
	if (ROMBOT == 0x0) {
#ifdef BOOTLOADER_REMAP_VECTOR_TABLE
		uint32_t offset = BOOTLOADER_REMAP_VECTOR_TABLE;
#else
		uint32_t offset = 0;
#endif
		flash_ROM(image, offset, num_bytes);
		return;
	}
#elif defined (HAVE_RAM)
	flash_RAM(image, 0, num_bytes);
	return;
#else
	// Enter debugging to circumvent state-tracking code and write directly
	state_enter_debugging();
	unsigned i;
	for (i=0; i<num_bytes; i++) {
		write_byte(i, image[i]);
	}
	state_exit_debugging();
	INFO("Wrote %d bytes to memory\n", num_bytes);
#endif
}

static void load_binary_file(const char* filename) {
	int flashfd = open(filename, O_RDONLY);
	ssize_t ret;

	if (-1 == flashfd) {
		ERR(E_BAD_FLASH, "Could not open '%s' for reading\n",
				filename);
	}

	struct stat flash_stat;
	if (0 != fstat(flashfd, &flash_stat)) {
		ERR(E_BAD_FLASH, "Could not get flash file size: %s\n",
				strerror(errno));
	}

#ifdef HAVE_ROM
	// There is no portable format specifier for an off_t, but it is defined to
	// be signed. So we cast is to a long long and move on with life.
	if (flash_stat.st_size > ROMSIZE)
		ERR(E_BAD_FLASH, "Request file size (%08llx) exceeds rom size (%08x)\n",
				(long long) flash_stat.st_size, ROMSIZE);
#else
	if (flash_stat.st_size > RAMSIZE)
		ERR(E_BAD_FLASH, "Request file size (%08llx) exceeds ram size (%08x)\n",
				(long long) flash_stat.st_size, RAMSIZE);
#endif

	{
		uint8_t flash[flash_stat.st_size];

		ret = read(flashfd, flash, flash_stat.st_size);
		if (ret < 0) {
			WARN("%s\n", strerror(errno));
			ERR(E_BAD_FLASH, "Failed to read flash file '%s'\n",
					filename);
		}

		uint32_t image_size = (uint32_t) ret;
		assert(image_size == ret);
		flash_image(flash, image_size);
	}

	INFO("Succesfully loaded image: %s\n", filename);
}

#ifdef __HAVE_LIBELF__
#ifdef DEBUG1
static const char* get_ptype(size_t pt) {
#define C(V) case PT_##V: return #V; break
	switch (pt) {
			C(NULL);
			C(LOAD);
			C(DYNAMIC);
			C(INTERP);
			C(NOTE);
			C(SHLIB);
			C(PHDR);
			C(TLS);
			C(NUM);

			C(SUNWBSS);
			C(SUNWCAP);
			C(SUNW_UNWIND);
			C(SUNWSTACK);
			C(SUNWDTRACE);

			C(ARM_EXIDX);
		default:
			if ( (pt >= PT_LOOS) && (pt <= PT_HIOS) )
				return "os-custom";
			if ( (pt >= PT_LOPROC) && (pt <= PT_HIPROC) )
				return "proc-custom";
			return "unknown";
	}
#undef C
}
#endif

static void load_elf_file(const char* filename) {
	int fd;
	Elf *e;
	size_t phdr_count;
	GElf_Phdr phdr;

	WARN("ELF loading is new. It may break unexpectedly.\n");

	if (elf_version(EV_CURRENT) == EV_NONE) {
		ERR(E_BAD_FLASH, "ELF library initialization: %s\n",
				elf_errmsg(-1));
	}
	if ((fd = open(filename, O_RDONLY, 0)) < 0) {
		ERR(E_BAD_FLASH, "Failed to open file %s: %s\n",
				filename, strerror(errno));
	}
	if ((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL) {
		ERR(E_BAD_FLASH, "elf_begin failed: %s\n", elf_errmsg(-1));
	}
	if (elf_kind(e) != ELF_K_ELF) {
		ERR(E_BAD_FLASH, "%s is not an ELF object\n", filename);
	}

	if (elf_getphdrnum(e, &phdr_count) != 0) {
		ERR(E_BAD_FLASH, "Failed to get program header count: %s\n",
				elf_errmsg(-1));
	}

	size_t i;
	for (i = 0; i < phdr_count; i++) {
		if (gelf_getphdr(e, i, &phdr) != &phdr) {
			ERR(E_BAD_FLASH, "getphdr faild: %s\n",
					elf_errmsg(-1));
		}

#define PHDR_FIELD(F) do {\
	DBG1("\t%-20s 0x%jx\n", #F, (uintmax_t) phdr.F);\
} while (0)
		DBG1("PHDR %lu:\n", i);
		PHDR_FIELD(p_type);
		DBG1("\t  [%s]\n", get_ptype(phdr.p_type));
		PHDR_FIELD(p_offset);
		PHDR_FIELD(p_vaddr);
		PHDR_FIELD(p_paddr);
		PHDR_FIELD(p_filesz);
		PHDR_FIELD(p_memsz);
		PHDR_FIELD(p_flags);
		DBG1("\t  [%s,%s,%s]\n"
				,(phdr.p_flags & PF_X) ? "execute":""
				,(phdr.p_flags & PF_R) ? "read":""
				,(phdr.p_flags & PF_W) ? "write":""
		    );
		PHDR_FIELD(p_align);
#undef PHDR_FIELD

		if (phdr.p_type == PT_LOAD) {
			uint8_t data[phdr.p_memsz];
			memset(data, 0, phdr.p_memsz);

			if (lseek(fd, phdr.p_offset, SEEK_SET) == -1) {
				ERR(E_BAD_FLASH, "error seeking: %s\n",
						strerror(errno));
			}
			if (read(fd, data, phdr.p_filesz) == -1) {
				ERR(E_BAD_FLASH, "error reading segment: %s\n",
						strerror(errno));
			}

			// XXX: This won't always be in flash
#ifdef HAVE_ROM
			flash_ROM(data, phdr.p_paddr, phdr.p_memsz);
#else
			flash_RAM(data, phdr.p_paddr, phdr.p_memsz);
#endif
		}
	}

	elf_end(e);
	close(fd);
}
#endif // __HAVE_LIBELF__

EXPORT void load_file(const char* filename) {
	const char* dot = strrchr(filename, '.');
	if (dot == NULL) {
		WARN("No file extension. Guessing binary image.\n");
		return load_binary_file(filename);
	}
#ifdef __HAVE_LIBELF__
	if (strcmp(dot+1, "elf") == 0) {
		return load_elf_file(filename);
	}
#endif // __HAVE_LIBELF__
	if (strcmp(dot+1, "bin") == 0) {
		return load_binary_file(filename);
	}
	WARN("Unknown file extension (%s). Guessing binary image.\n", dot+1);
	return load_binary_file(filename);
}

