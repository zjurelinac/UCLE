module.exports = (monaco) => {

	var keywords = [
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
	];

	monaco.languages.register({
		id: 's',
		extensions: [
			'.s',
		]
	});

	function getFriscCompletionProvider() {
		return {
			
		};
	}

	function getAreaInfo(text) {
		var item = ';';
		var isCompletionAvailable = true;

		text = text.replace(/;.*\n/g, "");

		var itemIdx = text.indexOf(item);
		if (itemIdx > -1) {
			text = text.substring(0, itemIdx);
			isCompletionAvailable = false;
		}
		return {
			isCompletionAvailable: isCompletionAvailable,
			clearedText: text
		};
	}

	function checkIfWordBasedSuggestion(textUntilPosition) {
		return keywords.some(function(keyword) {
			if(keyword.includes(textUntilPosition)) {
				return true;
			}
		});
	}

	function checkIfCond(textUntilPosition) {
		return (textUntilPosition.charAt(textUntilPosition.length-1) == '_');
	}

	monaco.languages.registerCompletionItemProvider('s', {
		triggerCharacters:[' ', '_'],
		provideCompletionItems: function (model, position) {
				// get editor content before the pointer
				let textUntilPosition = model.getValueInRange({startLineNumber: position.lineNumber, startColumn: 1, endLineNumber: position.lineNumber, endColumn: position.column});
				let info = getAreaInfo(textUntilPosition); // isCompletionAvailable, clearedText

				if(!info.isCompletionAvailable || !checkIfWordBasedSuggestion(textUntilPosition)) {
					return [];
				} else if(checkIfCond(textUntilPosition)) {
					console.log("usao");
					return [{label: textUntilPosition + 'C'},{label: textUntilPosition + 'NC'},
						{label: textUntilPosition + 'V'},{label: textUntilPosition + 'NV'},
						{label: textUntilPosition + 'N'},{label: textUntilPosition + 'NN'},
						{label: textUntilPosition + 'M'},{label: textUntilPosition + 'P'},
						{label: textUntilPosition + 'Z'},{label: textUntilPosition + 'NZ'},
						{label: textUntilPosition + 'EQ'},{label: textUntilPosition + 'NE'},
						{label: textUntilPosition + 'ULE'},{label: textUntilPosition + 'UGT'},
						{label: textUntilPosition + 'ULT'},{label: textUntilPosition + 'UGE'},
						{label: textUntilPosition + 'SLE'},{label: textUntilPosition + 'SGT'},
						{label: textUntilPosition + 'SLT'},{label: textUntilPosition + 'SGE'}];
				}

				return [
					{
						label:"ADD src1, src2, dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "src1 + src2 -> dest",
						insertText: {
							value: "ADD ${1:src1}, ${2:src2}, ${3:dest}$0"
						}
					},
					{
						label:"ADC src1, src2, dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "src1 + src2 + C -> dest",
						insertText: {
							value: "ADC ${1:src1}, ${2:src2}, ${3:dest}$0"
						}
					},
					{
						label:"SUB src1, src2, dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "src1 - src2 -> dest",
						insertText: {
							value: "SUB ${1:src1}, ${2:src2}, ${3:dest}$0"
						}
					},
					{
						label:"SBC src1, src2, dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "src1 - src2 + C -> dest",
						insertText: {
							value: "SBC ${1:src1}, ${2:src2}, ${3:dest}$0"
						}
					},
					{
						label:"CMP src1, src2",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "src1 - src2",
						insertText: {
							value: "CMP ${1:src1}, ${2:src2}$0"
						}
					},
					{
						label:"AND src1, src2, dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "src1 AND src -> dest",
						insertText: {
							value: "AND ${1:src1}, ${2:src2}, ${3:dest}$0"
						}
					},
					{
						label:"OR src1, src2, dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "src1 OR src2 -> dest",
						insertText: {
							value: "OR ${1:src1}, ${2:src2}, ${3:dest}$0"
						}
					},
					{
						label:"XOR src1, src2, dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "src1 XOR src2 -> dest",
						insertText: {
							value: "XOR ${1:src1}, ${2:src2}, ${3:dest}$0"
						}
					},
					{
						label:"SHL src1, src2, dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "",
						insertText: {
							value: "SHL ${1:src1}, ${2:src2}, ${3:dest}$0"
						}
					},
					{
						label:"SHR src1, src2, dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "",
						insertText: {
							value: "SHR ${1:src1}, ${2:src2}, ${3:dest}$0"
						}
					},
					{
						label:"ASHR src1, src2, dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "",
						insertText: {
							value: "ADD ${1:src1}, ${2:src2}, ${3:dest}$0"
						}
					},
					{
						label:"ROTL src1, src2, dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "",
						insertText: {
							value: "ROTL ${1:src1}, ${2:src2}, ${3:dest}$0"
						}
					},
					{
						label:"ROTR src1, src2, dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "src1 + src2 + C -> dest",
						insertText: {
							value: "ADD ${1:src1}, ${2:src2}, ${3:dest}$0"
						}
					},
					{
						label:"MOVE src2, dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "src2 -> dest",
						insertText: {
							value: "MOVE ${1:src2}, ${2:dest}$0"
						}
					},
					{
						label:"MOVE SR, dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "SR -> dest",
						insertText: {
							value: "MOVE SR, ${1:dest}$0"
						}
					},
					{
						label:"MOVE src2, SR",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "src2 -> SR",
						insertText: {
							value: "MOVE ${1:src2}, SR$0"
						}
					},
					{
						label:"LOAD{B,H} dest, (adr20)",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "MEM(ext adr20) -> dest",
						insertText: {
							value: "LOAD${1| ,B ,H |}, (${2:adr20})$0"
						},
					},
					{
						label:"LOAD{B,H} dest, (adrreg + offset20)",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "MEM(adrreg + ext offset20) -> dest",
						insertText: {
							value: "LOAD${1| ,B ,H |}, (${2:adrreg} + ${3:offset20})$0"
						}
					},
					{
						label:"STORE{B,H} src1, (adr20)",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "src1 -> MEM(ext adr20)",
						insertText: {
							value: "STORE${1| ,B ,H |}, (${2:adr20})"
						}
					},
					{
						label:"STORE{B,H} src1, (adrreg + offset20)",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "src1 -> MEM(adrreg + ext offset20)",
						insertText: {
							value: "STORE${1| ,B ,H |}, (${2:adrreg} + ${3:offset20}})$0"
						}
					},
					{
						label:"PUSH src1",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "R7 – 4 -> R7; src1 -> MEM(R7)",
						insertText: {
							value: "PUSH ${1:src1}$0"
						}
					},
					{
						label:"POP dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "MEM(R7) -> dest;  R7 + 4 -> R7",
						insertText: {
							value: "POP ${1:dest}$0"
						}
					},
					{
						label:"JP{cond} adr20",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "ext adr20 -> PC",
						insertText: {
							value: "JP${1| ,_C ,_NC ,_V ,_NV ,_N ,_NN ,_M ,_P ,_Z ,_NZ ,_EQ ," + 
									"_NE ,_ULE ,_UGT ,_ULT ,_UGE ,_SLE ,_SGT ,_SLT ,_SGE |}$0"
						}
					},
					{
						label:"JP{cond} (adrreg)",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "adrreg -> PC",
						insertText: {
							value: "JP${1| ,_C ,_NC ,_V ,_NV ,_N ,_NN ,_M ,_P ,_Z ,_NZ ,_EQ ," + 
									"_NE ,_ULE ,_UGT ,_ULT ,_UGE ,_SLE ,_SGT ,_SLT ,_SGE |}(${2:adrreg})$0"
						}
					},
					{
						label:"JR{cond} adr",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "PC + ext offset20 -> PC",
						insertText: {
							value: "JR${1| ,_C ,_NC ,_V ,_NV ,_N ,_NN ,_M ,_P ,_Z ,_NZ ,_EQ ," + 
									"_NE ,_ULE ,_UGT ,_ULT ,_UGE ,_SLE ,_SGT ,_SLT ,_SGE |}$0"
						}
					},
					{
						label:"CALL{cond} adr20",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "R7 – 4 -> R7; PC -> MEM(R7); ext adr20 -> PC",
						insertText: {
							value: "CALL${1| ,_C ,_NC ,_V ,_NV ,_N ,_NN ,_M ,_P ,_Z ,_NZ ,_EQ ," + 
									"_NE ,_ULE ,_UGT ,_ULT ,_UGE ,_SLE ,_SGT ,_SLT ,_SGE |}$0"
						}
					},
					{
						label:"CALL{cond} (adrreg)",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "R7 – 4 -> R7; PC -> MEM(R7); adrreg -> PC",
						insertText: {
							value: "CALL${1| ,_C ,_NC ,_V ,_NV ,_N ,_NN ,_M ,_P ,_Z ,_NZ ,_EQ ," + 
									"_NE ,_ULE ,_UGT ,_ULT ,_UGE ,_SLE ,_SGT ,_SLT ,_SGE |}(${2:adrreg})$0"
						}
					},
					{
						label:"RET{cond}",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "MEM(R7) -> PC; R7 + 4 -> R7",
						insertText: {
							value: "RET${1|_C,_NC,_V,_NV,_N,_NN,_M,_P,_Z,_NZ,_EQ," + 
									"_NE,_ULE,_UGT,_ULT,_UGE,_SLE,_SGT,_SLT,_SGE|}$0"
						}
					},
					{
						label:"RETI{cond}",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "MEM(R7) -> PC; R7 + 4 -> R7; 1 -> GIE",
						insertText: {
							value: "RETI${1|_C,_NC,_V,_NV,_N,_NN,_M,_P,_Z,_NZ,_EQ," + 
									"_NE,_ULE,_UGT,_ULT,_UGE,_SLE,_SGT,_SLT,_SGE|}$0"
						}
					},
					{
						label:"RETN{cond}",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "MEM(R7) -> PC; R7 + 4 -> R7; 1 -> IIF",
						insertText: {
							value: "RETN${1|_C,_NC,_V,_NV,_N,_NN,_M,_P,_Z,_NZ,_EQ," + 
									"_NE,_ULE,_UGT,_ULT,_UGE,_SLE,_SGT,_SLT,_SGE|}$0"
						}
					},
					{
						label:"HALT{cond}",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "Stop the processor",
						insertText: {
							value: "HALT${1|_C,_NC,_V,_NV,_N,_NN,_M,_P,_Z,_NZ,_EQ," + 
									"_NE,_ULE,_UGT,_ULT,_UGE,_SLE,_SGT,_SLT,_SGE|}$0"
						}
					},
				];
			}
		}
	);

	monaco.languages.setMonarchTokensProvider('s', {
		keywords: keywords,

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