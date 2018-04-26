module.exports = (monaco) => {
	monaco.languages.register({
		id: 's',
		extensions: [
			'.s',
		]
	});

	monaco.languages.setMonarchTokensProvider('s', {
		keywords: [
			'ADD', 'ADC', 'SUB', 'SBC', 'CMP', 'AND', 'OR',
			'XOR', 'SHL', 'SHR', 'ASHR', 'ROTL', 'ROTR', 'MOVE',
			'LOAD', 'LOADB', 'LOADH', 'STORE', 'STOREB', 'STOREH',
			'PUSH', 'POP', 'JP', 'JR', 'CALL', 'RET', 'RETI', 'RETN',
			'WRST', 'RDST', 'NOP', 'CLEAR', 'COMPL', 'NEG',
			'SUBR', 'INC', 'DEC', 'TEST', 'HALT'
		],

		typeKeywords: [
			'EQU', 'DW', 'DH', 'DB', 'DS'
		],

		types: [
			'R0','R1', 'R2', 'R3', 'R4', 'R5', 'R6', 'R7'
		],

		brackets: [
			['(',')','delimiter.parenthesis']
		],

		builtins: [
			'SP', 'SR', 'PC'
		],

		// we include these common regular expressions
		symbols: /[=><!~?:&|+\-*\/\^%]+/,

		// The main tokenizer for our languages
		tokenizer: {
			root: [
				[/[A-Z]*[a-z]_*/,'white' ],
				[/[A-Z]\w*/, { 
					cases: { 
						'@typeKeywords': 'keyword',

						'@keywords': 'keyword',

						'@types': 'identifier',

						'@builtins': 'string',

						'@default': 'type.identifier' 
					} 
				}],
				{ include: '@whitespace' },
				// numbers
				[/0[xX][0-9a-fA-F]+/, 'number.hex'],
				[/\d+/, 'number'],
			],
			whitespace: [
				[/[ \t\r\n]+/, 'white'],
				[/;.*/, 'comment']
			]
		}
	});
}