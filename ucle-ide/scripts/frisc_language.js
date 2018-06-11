module.exports = (monaco) => {

	const keywords = [
			'ADD', 'ADC', 'SUB', 'SBC', 'CMP', 'AND', 'OR',
			'XOR', 'SHL', 'SHR', 'ASHR', 'ROTL', 'ROTR', 'MOVE',
			'LOAD', 'LOADB', 'LOADH', 'STORE', 'STOREB', 'STOREH',
			'PUSH', 'POP',
			'JP', 'JP_C', 'JP_NC', 'JP_V', 'JP_NV',
			'JP_N', 'JP_NN', 'JP_M', 'JP_P', 'JP_Z', 'JP_NZ', 'JP_EQ',
			'JP_NE', 'JP_ULE', 'JP_UGT', 'JP_ULT', 'JP_UGE', 'JP_SLE',
			'JP_SGT', 'JP_SLT', 'JP_SGE',
			'JR', 'JR_C', 'JR_NC', 'JR_V', 'JR_NV', 'JR_N', 'JR_NN',
			'JR_M', 'JR_P', 'JR_Z', 'JR_NZ', 'JR_EQ', 'JR_NE', 'JR_ULE',
			'JR_UGT', 'JR_ULT', 'JR_UGE', 'JR_SLE', 'JR_SGT', 'JR_SLT', 'JR_SGE',
			'CALL', 'CALL_C', 'CALL_NC', 'CALL_V', 'CALL_NV',
			'CALL_N', 'CALL_NN', 'CALL_M', 'CALL_P', 'CALL_Z',
			'CALL_NZ', 'CALL_EQ', 'CALL_NE', 'CALL_ULE',
			'CALL_UGT','CALL_ULT', 'CALL_UGE', 'CALL_SLE',
			'CALL_SGT', 'CALL_SLT', 'CALL_SGE',
			'RET', 'RET_C', 'RET_NC', 'RET_V', 'RET_NV',
			'RET_N', 'RET_NN', 'RET_M', 'RET_P', 'RET_Z',
			'RET_NZ', 'RET_EQ', 'RET_NE', 'RET_ULE',
			'RET_UGT','RET_ULT', 'RET_UGE', 'RET_SLE',
			'RET_SGT', 'RET_SLT', 'RET_SGE',
			'RETI', 'RETI_C', 'RETI_NC', 'RETI_V', 'RETI_NV',
			'RETI_N', 'RETI_NN', 'RETI_M', 'RETI_P', 'RETI_Z',
			'RETI_NZ', 'RETI_EQ', 'RETI_NE', 'RETI_ULE',
			'RETI_UGT','RETI_ULT', 'RETI_UGE', 'RETI_SLE',
			'RETI_SGT', 'RETI_SLT', 'RETI_SGE',
			'RETN', 'RETN_C', 'RETN_NC', 'RETN_V', 'RETN_NV',
			'RETN_N', 'RETN_NN', 'RETN_M', 'RETN_P', 'RETN_Z',
			'RETN_NZ', 'RETN_EQ', 'RETN_NE', 'RETN_ULE',
			'RETN_UGT','RETN_ULT', 'RETN_UGE', 'RETN_SLE',
			'RETN_SGT', 'RETN_SLT', 'RETN_SGE',
			'HALT', 'HALT_C', 'HALT_NC', 'HALT_V', 'HALT_NV',
			'HALT_N', 'HALT_NN', 'HALT_M', 'HALT_P', 'HALT_Z',
			'HALT_NZ', 'HALT_EQ', 'HALT_NE', 'HALT_ULE',
			'HALT_UGT','HALT_ULT', 'HALT_UGE', 'HALT_SLE',
			'HALT_SGT', 'HALT_SLT', 'HALT_SGE'
	];

	monaco.languages.register({
		id: 'frisc-assembly',
		extensions: [
			'.s',
			'.asm'
		]
	});

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

	monaco.languages.registerCompletionItemProvider('frisc-assembly', {
		triggerCharacters:['_'],
		provideCompletionItems: function (model, position) {
				let textUntilPosition = model.getValueInRange({startLineNumber: position.lineNumber, startColumn: 1, endLineNumber: position.lineNumber, endColumn: position.column});
				let info = getAreaInfo(textUntilPosition);

				if(!info.isCompletionAvailable || !checkIfWordBasedSuggestion(textUntilPosition)) {
					return [];
				} else if(checkIfCond(textUntilPosition)) {
					return [
						{label: textUntilPosition + 'C', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'NC', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'V', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'NV', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'N', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'NN', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'M', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'P', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'Z', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'NZ', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'EQ', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'NE', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'ULE', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'UGT', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'ULT', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'UGE', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'SLE', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'SGT', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'SLT', kind: monaco.languages.CompletionItemKind.Function},
						{label: textUntilPosition + 'SGE', kind: monaco.languages.CompletionItemKind.Function}
					];
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
						documentation: "Logic shift left src1 by number of bits in src2 -> dest",
						insertText: {
							value: "SHL ${1:src1}, ${2:src2}, ${3:dest}$0"
						}
					},
					{
						label:"SHR src1, src2, dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "Logic shift right src1 by number of bits in src2 -> dest",
						insertText: {
							value: "SHR ${1:src1}, ${2:src2}, ${3:dest}$0"
						}
					},
					{
						label:"ASHR src1, src2, dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "Arithmetic shift right src1 by number of bits in src2 -> dest",
						insertText: {
							value: "ASHR ${1:src1}, ${2:src2}, ${3:dest}$0"
						}
					},
					{
						label:"ROTL src1, src2, dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "Rotate left src1 by number of bits in src2 -> dest",
						insertText: {
							value: "ROTL ${1:src1}, ${2:src2}, ${3:dest}$0"
						}
					},
					{
						label:"ROTR src1, src2, dest",
						kind: monaco.languages.CompletionItemKind.Function,
						documentation: "Rotate right src1 by number of bits in src2 -> dest",
						insertText: {
							value: "ROTR ${1:src1}, ${2:src2}, ${3:dest}$0"
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

	monaco.languages.setMonarchTokensProvider('frisc-assembly', {
		keywords: keywords,

		typeKeywords: [
			'ORG', 'EQU', 'DW', 'DH', 'DB', 'DS'
		],

		types: [
			'R0','R1', 'R2', 'R3', 'R4', 'R5', 'R6'
		],

		brackets: [
			['(',')','delimiter.parenthesis']
		],

		builtins: [
			'R7', 'SP', 'SR', 'PC'
		],

		tokenizer: {
			root: [
				//[/[A-Z]*[a-z]_*/,'white'],
				[/\_*[0-9a-zA-Z]\w*/, { 
					cases: { 
						'@typeKeywords': 'keyword',

						'@keywords': 'keyword',

						'@types': 'identifier',

						'@builtins': 'string',

						'@default': 'type.identifier' 
					} 
				}],
				{ include: '@whitespace' },
				// numbers and base
				[/[%][B|O|D|H]+/, 'keyword'],
				[/\d+/, 'number'],
			],
			whitespace: [
				[/[ \t\r\n]+/, 'white'],
				[/;.*/, 'comment']
			]
		}
	});
}