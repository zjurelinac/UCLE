const cp = require('child_process');
const path = require('path');
const fs = require('fs');

var procSim = path.resolve("../ucle/build/core/debug/", './fnsim-cli');

//var procAssembler = path.resolve(...);

var child;
var infoInterval;
var simFileModel;

var quitCommand = {"command": "quit", "args": []};

var infoCommand = {"command": "info", "args": []};

var startCommand = {"command": "start", "args":[]};

var decorations = [];

class UCLEServer {
	constructor({ editor, monaco }) {
		this.editor = editor;
		this.monaco = monaco;
	}

	init() {
		this.editor.onMouseDown(function(e) {
			if(e.target.type == 2) {
				var model = this.editor.getModel();
				console.log(model.getLineContent(e.target.position.lineNumber));
				if(model == null || model == undefined) return;

				var line = e.target.position.lineNumber;

				if(!model.getLineContent(line).replace(/\s/g, '').length) return;

				var	column = e.target.position.column;
				var range = new this.monaco.Range(line, column, line, column);
				var address = simFileModel.getLineContent(line).split(" ")[0];

				var dec = model.getLineDecorations(line);

				var remove = dec.some(function(e, index) {
					if(e.options.glyphMarginClassName == "breakpoint") {
						model.deltaDecorations([e.id], [
							{
								range: range,
								options: {
									glyphMarginClassName:"breakpoint-hover",
									stickiness: e.options.stickiness
								}
							}
						]);
						decorations.splice(decorations.indexOf(e.id),1);
						return true;
					} else if(e.options.glyphMarginClassName == "breakpoint-hover") {
						model.deltaDecorations([e.id], []);
						return false;
					}
				});

				if(remove) {
					this.sendCommand("break", ["del", parseInt(address, 16)]);
					return;
				}

				var stick = this.monaco.editor.TrackedRangeStickiness.GrowsOnlyWhenTypingBefore;

				if(model.getLineContent(line) != "") {
					stick = this.monaco.editor.TrackedRangeStickiness.NeverGrowsWhenTypingAtEdges;
				}

				var newDecorations = model.deltaDecorations([], [
					{
						range: range,
						options: {
							glyphMarginClassName: 'breakpoint',
							stickiness: stick
						}
					}
				]);

				var address = simFileModel.getLineContent(line).split(" ")[0];

				console.log(address);

				this.sendCommand("break", ["add", parseInt(address, 16)]);

				decorations.push(newDecorations[0]);
			}
		}, this);

		this.editor.onMouseMove(function(e) {
			var model = this.editor.getModel();
			if(model == null || model == undefined) return;

			if(e.target.type == 2) {
				var line = e.target.position.lineNumber;
				this.removeHoverBreakPoint(model);

				if(!model.getLineContent(line).replace(/\s/g, '').length) {
					return;
				}

				var	column = e.target.position.column;
				var range = new this.monaco.Range(line, column, line, column);

				if(this.checkIfBreakPoint(model, line)) {
					return;
				}

				model.deltaDecorations([], [
					{
						range: range,
						options: {
							glyphMarginClassName: 'breakpoint-hover'
						}
					}
				]);
			} else {
				this.removeHoverBreakPoint(model);
			}

		}, this);

		this.editor.onMouseLeave(function(e) {
			var model = this.editor.getModel();
			if(model == null || model == undefined) return;
			this.removeHoverBreakPoint(model);
		},this);

	}

	checkIfBreakPoint(model,line) {
		var dec = model.getLineDecorations(line);

		return dec.some(function(e, index) {
			if(e.options.glyphMarginClassName == "breakpoint") {
				return true;
			}
		});
	}

	removeHoverBreakPoint(model) {
		var allDec = model.getAllDecorations();

		allDec.forEach(function(e, index) {
			if(e.options.glyphMarginClassName == "breakpoint-hover") {
				model.deltaDecorations([e.id], []);
				return;
			}
		});
	}

	addHighLight(model, range) {
		this.removeHighLight(model);

		model.deltaDecorations([], [
			{
				range: range,
				options: {
					isWholeLine: true,
					className: 'highlight'
				}
			}
		]);
	}

	removeHighLight(model) {
		var allDec = model.getAllDecorations();

		allDec.some(function(e, index) {
			if(e.options.className == "highlight") {
				model.deltaDecorations([e.id], []);
			}
		});
	}

	registerBreakPoints(model) {
		this.editor.updateOptions({glyphMargin:true});

		var allDec = model.getAllDecorations();

		allDec.forEach(function(e) {
			if(e.options.glyphMarginClassName == "breakpoint") {
				var address = simFileModel.getLineContent(e.range.startLineNumber).split(" ")[0];
				this.sendCommand("break", ["add", parseInt(address,16)]);
			}
		}, this);
	}

	removeBreakPoints(model) {
		var allDec = model.getAllDecorations();

		allDec.forEach(function(e) {
			if(e.options.glyphMarginClassName == "breakpoint") {
				var address = simFileModel.getLineContent(e.range.startLineNumber).split(" ")[0];
				this.sendCommand("break", ["del", parseInt(address,16)]);
			}
		}, this);
		decorations = model.deltaDecorations(decorations, []);
	}

	findFirstNonEmpty(model, line, address) {
		var nextLine = line + 1;
		var lineCount = model.getLineCount();

		if((nextLine > lineCount) || !nextLine) return -1;

		var content = model.getLineContent(nextLine);

		if(!content.replace(/\s/g, '').length) {
			for(var i = 1; i <= lineCount; i++) {
				if(model.getLineContent(i).replace(/\s/g, '').length) {
					var nextAddress = simFileModel.getLineContent(i).split(" ")[0];
					if(parseInt(nextAddress,16) == address) {
						return i;
					}
				}
			}
		}
		return -1;
	}

	findFirstBreakPoint(model, line, address) {
		var nextLine = line + 1;
		var lineCount = model.getLineCount();

		if((nextLine > lineCount) || !nextLine) return -1;

		for(var i = 1; i <= lineCount; i++) {
			if(this.checkIfBreakPoint(model, i)) {
				var nextAddress = simFileModel.getLineContent(i).split(" ")[0];
				console.log(parseInt(nextAddress,16));
				console.log(address);
				if(parseInt(nextAddress,16) >= address) {
					return i;
				}
			}
		}		
		return -1;
	}

	getMachineCode(filePath) {
		/*child = cp.exec(procAssembler + " " + filePath, function(stderr) {
			console.log(stderr);
		}); 

		TODO with our assembler*/

		var data = fs.readFileSync(filePath, 'utf-8');

		simFileModel = this.monaco.editor.createModel(data);

		return filePath;
	}

	runSim(filePath) {
		var simPath = this.getMachineCode(filePath);

		child = cp.execFile(procSim, ['FRISC', simPath, '-j']);

		child.stdout.on('data', function(data) {
			require('electron').remote.getCurrentWindow().webContents.send("sim-response", data);
		});

		child.stdin.write(JSON.stringify(startCommand) + '\n');

		var sim = this;

		infoInterval = setInterval(function() {
			sim.sendCommand("info",[]);
		}, 500);
	}

	sendCommand(command, args) {
		var JSONcommand = { "command": command, "args": args};
		child.stdin.write(JSON.stringify(JSONcommand) + '\n');
	}

	stopSim() {
		child.stdin.write(JSON.stringify(quitCommand) + '\n');
		this.editor.updateOptions({glyphMargin:false});
		clearInterval(infoInterval);
		simFileModel.dispose();
	}
}

module.exports = UCLEServer;