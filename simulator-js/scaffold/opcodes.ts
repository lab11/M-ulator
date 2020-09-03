export function match_mask8(inst: number, ones_mask: number, zeros_mask: number): Boolean {
    inst = inst & 0xff;
    ones_mask = ones_mask & 0xff;
    zeros_mask = zeros_mask & 0xff;
    return (ones_mask == (ones_mask & inst)) && (zeros_mask == (zeros_mask & ~inst));
}

export function match_mask16(inst: number, ones_mask: number, zeros_mask: number): Boolean {
    inst = inst & 0xffff;
    ones_mask = ones_mask & 0xffff;
    zeros_mask = zeros_mask & 0xffff;
    return (ones_mask == (ones_mask & inst)) && (zeros_mask == (zeros_mask & ~inst));
}

export function match_mask32(inst: number, ones_mask: number, zeros_mask: number): Boolean {
    inst = inst & 0xffff_ffff;
    ones_mask = ones_mask & 0xffff_ffff;
    zeros_mask = zeros_mask & 0xffff_ffff;
    return (ones_mask == (ones_mask & inst)) && (zeros_mask == (zeros_mask & ~inst));
}
