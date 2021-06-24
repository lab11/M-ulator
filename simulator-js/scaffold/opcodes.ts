import assert = require("assert");

export type Op16Function = (inst: number) => void;
export type Op32Function = (inst: number) => void;

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

export function find_op(inst: number): Op16Function | Op32Function {
    switch (inst & 0xf8000000) {
        case 0x00000000:
            assert((inst & 0xffff0000) == 0);
            olist = op16_root[(inst >> 8) & 0xff];
            while (olist != NULL) {
                // optimization
                //if (olist->next == NULL) return olist->o;

                if (match_op16(inst, olist -> o))
                    return olist -> o;
                olist = olist -> next;
            }
            return NULL;

        case 0xe8000000:
            olist = op32_11101_root[(inst >> 19) & 0xff];
            break;
        case 0xf0000000:
            olist = op32_11110_root[(inst >> 19) & 0xff];
            break;
        case 0xf8000000:
            olist = op32_11111_root[(inst >> 19) & 0xff];
            break;
    }

    while (olist != NULL) {
        // optimization
        // if (olist->next == NULL) return olist->o;

        if (match_op32(inst, olist -> o))
            return olist -> o;
        olist = olist -> next;
    }
    return NULL;
}