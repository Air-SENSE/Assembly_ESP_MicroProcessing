
.align	4

	// Define variables, which go into .bss section (zero-initialized data)
.section .bss



    // The section below is reserved for declaring variable.
.section .data



    // The section below is reserved for declaring functional.
    .text
    .global entry
    .type entry, @function
entry:
// This is the entry point of the assembly program
// It executes the main logic of the program
// Write your code here

    add r0, r1, r0

	/* End program */
    halt

