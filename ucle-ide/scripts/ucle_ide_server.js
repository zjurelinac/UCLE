var cp = require('child_process');
var path = require('path');

var proc = path.resolve("../ucle/build/core/debug/", './fnsim-cli');
var child;

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
				if(model == null || model == undefined) return;

				var line = e.target.position.lineNumber;
				var	column = e.target.position.column;
				var range = new this.monaco.Range(line, column, line, column);

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

				decorations.push(newDecorations[0]);
				console.log(model.getLineDecorations(line));
			}
		}, this);

		this.editor.onMouseMove(function(e) {
			var model = this.editor.getModel();
			if(model == null || model == undefined) return;

			if(e.target.type == 2) {
				var line = e.target.position.lineNumber;
				var	column = e.target.position.column;
				var range = new this.monaco.Range(line, column, line, column);

				var dec = model.getLineDecorations(line);

				var allDec = model.getAllDecorations();

				this.removeHoverBreakPoint(model);

				var remove = dec.some(function(e, index) {
					if(e.options.glyphMarginClassName == "breakpoint") {
						return true;
					}
				});

				if(remove) {
					return;
				}

				var stick = this.monaco.editor.TrackedRangeStickiness.GrowsOnlyWhenTypingBefore;

				if(model.getLineContent(line) != "") {
					stick = this.monaco.editor.TrackedRangeStickiness.NeverGrowsWhenTypingAtEdges;
				}

				model.deltaDecorations([], [
					{
						range: range,
						options: {
							glyphMarginClassName: 'breakpoint-hover',
							stickiness: stick
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

	removeHoverBreakPoint(model) {
		var allDec = model.getAllDecorations();

		var del = allDec.some(function(e, index) {
			if(e.options.glyphMarginClassName == "breakpoint-hover") {
				model.deltaDecorations([e.id], []);
			}
		});
	}

	registerBreakPoints() {
		this.editor.updateOptions({glyphMargin:true});
	}


	runSim(filePath) {
		var file = path.resolve(__dirname, "../test/test.p");

		child = cp.execFile(proc, ['FRISC', file, '-j']);

		child.stdout.on('data', function(data) {
			require('electron').remote.getCurrentWindow().webContents.send("sim-response", data.toString());
		});

		child.stdin.write(JSON.stringify(startCommand) + '\n');
	}

	sendCommand(command, args) {
		var JSONcommand = { "command": command, "args": args};
		child.stdin.write(JSON.stringify(JSONcommand) + '\n');
	}

	stopSim() {
		child.stdin.write(JSON.stringify(quitCommand) + '\n');
		this.editor.updateOptions({glyphMargin:false});
	}
}

module.exports = UCLEServer;