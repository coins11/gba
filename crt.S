//
// c r t . S
//
// The simplest C runtime startup routine for GBA
//
// Apr. 30, 2003 by Wataru Nishida (http://www.wnishida.com)
//

.text				// Select .text section
	b	main		// Branch to main() in C source
