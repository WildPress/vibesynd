// Pre-analysis: turn ON aggressive coverage options and seed disassembly, so
// auto-analysis attributes far more of the raw code segment to functions.
// Raw-binary analysis otherwise misses indirectly-reached code (jump tables,
// function pointers, vtables) and gap code it never flows into.

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;

public class AggressiveSeed extends GhidraScript {
    // LE object-1 entry: EIP offset within the code object (base 0x10000).
    private static final long EIP_OFFSET = 0x2d85cL;

    @Override
    public void run() throws Exception {
        // Fill gaps and chase code the normal flow never reaches.
        setAnalysisOption(currentProgram, "Aggressive Instruction Finder", "true");
        // Decompiler-driven function start discovery.
        setAnalysisOption(currentProgram, "Decompiler Parameter ID", "true");

        Address base = currentProgram.getImageBase();
        disassemble(base);

        // Seed from the real program entry point so flow analysis reaches the
        // bulk of the code (only meaningful once loaded at the correct base).
        Address entry = base.add(EIP_OFFSET);
        disassemble(entry);
        createFunction(entry, "entry_point");
    }
}
