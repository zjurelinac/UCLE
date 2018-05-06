var cp = require('child_process');
var path = require('path');

var proc = path.resolve("../ucle/build/core/debug/", './fnsim-cli');
var child;

class UCLEServer {
	constructor({ editor, monaco }) {
		this.editor = editor;
		this.monaco = monaco;
	}

	runSim(filePath) {
		var file = path.resolve(__dirname, "../test/1.p");

		child = cp.execFile(proc, ['FRISC', file, '-j']);
		let readData = "";
		let finished = false;

		var helpCommand = {"command": "help", "args": []};

		var quitCommand = {"command": "quit", "args": []};

		var string = JSON.stringify(helpCommand);

		child.stdout.on('data', function(data) {
			readData += data.toString();
		});

		child.stdin.write(string + '\n');
		
		child.on('close', function() {
			require('electron').remote.getCurrentWindow().webContents.send("sim-response", readData);
		});

		setTimeout(function() {
			child.kill('SIGINT');
		}, 1000);
	}
}

module.exports = UCLEServer;