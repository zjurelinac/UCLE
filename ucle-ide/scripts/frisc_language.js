module.exports = (monaco) => {
	monaco.languages.register({
		id: 's',
		extensions: [
			'.s',
		]
	});

	function createDependencyProposals() {
		return [
			{
				label:"ADD",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "src1 + src2 -> dest",
				insertText: {
					value: "ADD ${1:src1}, ${2:src2}, ${3:dest}"
				}
			},
			{
				label:"ADC src1, src2, dest",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "src1 + src2 + C -> dest",
				insertText: {
					value: "ADC ${1:src1}, ${2:src2}, ${3:dest}"
				}
			},
			{
				label:"SUB src1, src2, dest",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "src1 - src2 -> dest",
				insertText: {
					value: "SUB ${1:src1}, ${2:src2}, ${3:dest}"
				}
			},
			{
				label:"SBC src1, src2, dest",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "src1 - src2 + C -> dest",
				insertText: {
					value: "SBC ${1:src1}, ${2:src2}, ${3:dest}"
				}
			},
			{
				label:"CMP src1, src2",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "src1 - src2",
				insertText: {
					value: "CMP ${1:src1}, ${2:src2}"
				}
			},
			{
				label:"AND src1, src2, dest",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "src1 AND src -> dest",
				insertText: {
					value: "AND ${1:src1}, ${2:src2}, ${3:dest}"
				}
			},
			{
				label:"OR src1, src2, dest",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "src1 OR src2 -> dest",
				insertText: {
					value: "OR ${1:src1}, ${2:src2}, ${3:dest}"
				}
			},
			{
				label:"XOR src1, src2, dest",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "src1 XOR src2 -> dest",
				insertText: {
					value: "XOR ${1:src1}, ${2:src2}, ${3:dest}"
				}
			},
			{
				label:"SHL src1, src2, dest",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "",
				insertText: {
					value: "SHL ${1:src1}, ${2:src2}, ${3:dest}"
				}
			},
			{
				label:"SHR src1, src2, dest",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "",
				insertText: {
					value: "SHR ${1:src1}, ${2:src2}, ${3:dest}"
				}
			},
			{
				label:"ASHR src1, src2, dest",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "",
				insertText: {
					value: "ADD ${1:src1}, ${2:src2}, ${3:dest}"
				}
			},
			{
				label:"ROTL src1, src2, dest",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "",
				insertText: {
					value: "ROTL ${1:src1}, ${2:src2}, ${3:dest}"
				}
			},
			{
				label:"ROTR src1, src2, dest",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "src1 + src2 + C -> dest",
				insertText: {
					value: "ADD ${1:src1}, ${2:src2}, ${3:dest}"
				}
			},
			{
				label:"MOVE src2, dest",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "src2 -> dest",
				insertText: {
					value: "MOVE ${1:src2}, ${2:dest}"
				}
			},
			{
				label:"MOVE SR, dest",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "SR -> dest",
				insertText: {
					value: "MOVE SR, ${1:dest}"
				}
			},
			{
				label:"MOVE src2, SR",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "src2 -> SR",
				insertText: {
					value: "MOVE ${1:src2}, SR"
				}
			},
			{
				label:"LOAD dest, (adr20)",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "MEM(ext adr20) -> dest",
				insertText: {
					value: "LOAD ${1:dest}, (${2:adr20})"
				}
			},
			{
				label:"LOADB dest, (adr20)",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "MEM(ext adr20) -> dest",
				insertText: {
					value: "LOADB ${1:dest}, (${2:adr20})"
				}
			},
			{
				label:"LOADH dest, (adr20)",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "MEM(ext adr20) -> dest",
				insertText: {
					value: "LOADH ${1:dest}, (${2:adr20})"
				}
			},
			{
				label:"ADC src1, src2, dest",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "src1 + src2 + C -> dest",
				insertText: {
					value: "ADD ${1:src1}, ${2:src2}, ${3:dest}"
				}
			},
			{
				label:"ADC src1, src2, dest",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "src1 + src2 + C -> dest",
				insertText: {
					value: "ADD ${1:src1}, ${2:src2}, ${3:dest}"
				}
			},
			{
				label:"ADC src1, src2, dest",
				kind: monaco.languages.CompletionItemKind.Function,
				documentation: "src1 + src2 + C -> dest",
				insertText: {
					value: "ADD ${1:src1}, ${2:src2}, ${3:dest}"
				}
			},
		];
	}

	monaco.languages.registerCompletionItemProvider('s', {
		triggerCharacters: [' '],
		provideCompletionItems: function(model, position, token) {
			return createDependencyProposals();
		}
	});

	monaco.languages.setMonarchTokensProvider('s', {
		keywords: [
			'ADD', 'ADC', 'SUB', 'SBC', 'CMP', 'AND', 'OR',
			'XOR', 'SHL', 'SHR', 'ASHR', 'ROTL', 'ROTR', 'MOVE',
			'LOAD', 'LOADB', 'LOADH', 'STORE', 'STOREB', 'STOREH',
			'PUSH', 'POP',
			'JP', 'JP_C', 'JP_NC', 'JP_V', 'JP_NV',
			'JP_N', 'JP_NN', 'JP_M', 'JP_N', 'JP_Z', 'JP_NZ', 'JP_EQ',
			'JP_NE', 'JP_ULE', 'JP_UGT', 'JP_ULT', 'JP_UGE', 'JP_SLE',
			'JP_SGT', 'JP_SLT', 'JP_SGE',
			'JR', 'JR_C', 'JR_NC', 'JR_V', 'JR_NV', 'JR_N', 'JR_NN',
			'JR_M', 'JR_N', 'JR_Z', 'JR_NZ', 'JR_EQ', 'JR_NE', 'JR_ULE',
			'JR_UGT', 'JR_ULT', 'JR_UGE', 'JR_SLE', 'JR_SGT', 'JR_SLT', 'JR_SGE',
			'CALL', 'CALL_C', 'CALL_NC', 'CALL_V', 'CALL_NV',
			'CALL_N', 'CALL_NN', 'CALL_M', 'CALL_N', 'CALL_Z',
			'CALL_NZ', 'CALL_EQ', 'CALL_NE', 'CALL_ULE',
			'CALL_UGT','CALL_ULT', 'CALL_UGE', 'CALL_SLE',
			'CALL_SGT', 'CALL_SLT', 'CALL_SGE',
			'RET', 'RET_C', 'RET_NC', 'RET_V', 'RET_NV',
			'RET_N', 'RET_NN', 'RET_M', 'RET_N', 'RET_Z',
			'RET_NZ', 'RET_EQ', 'RET_NE', 'RET_ULE',
			'RET_UGT','RET_ULT', 'RET_UGE', 'RET_SLE',
			'RET_SGT', 'RET_SLT', 'RET_SGE',
			'RETI', 'RETI_C', 'RETI_NC', 'RETI_V', 'RETI_NV',
			'RETI_N', 'RETI_NN', 'RETI_M', 'RETI_N', 'RETI_Z',
			'RETI_NZ', 'RETI_EQ', 'RETI_NE', 'RETI_ULE',
			'RETI_UGT','RETI_ULT', 'RETI_UGE', 'RETI_SLE',
			'RETI_SGT', 'RETI_SLT', 'RETI_SGE',
			'RETN', 'RETN_C', 'RETN_NC', 'RETN_V', 'RETN_NV',
			'RETN_N', 'RETN_NN', 'RETN_M', 'RETN_N', 'RETN_Z',
			'RETN_NZ', 'RETN_EQ', 'RETN_NE', 'RETN_ULE',
			'RETN_UGT','RETN_ULT', 'RETN_UGE', 'RETN_SLE',
			'RETN_SGT', 'RETN_SLT', 'RETN_SGE',
			'HALT', 'HALT_C', 'HALT_NC', 'HALT_V', 'HALT_NV',
			'HALT_N', 'HALT_NN', 'HALT_M', 'HALT_N', 'HALT_Z',
			'HALT_NZ', 'HALT_EQ', 'HALT_NE', 'HALT_ULE',
			'HALT_UGT','HALT_ULT', 'HALT_UGE', 'HALT_SLE',
			'HALT_SGT', 'HALT_SLT', 'HALT_SGE'
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