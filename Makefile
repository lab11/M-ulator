all: simulator programs

simulator:
	$(MAKE) -C simulator

programs:
	$(MAKE) -C programs

clean:
	$(MAKE) clean -C simulator
	$(MAKE) clean -C programs

.PHONY: simulator programs
