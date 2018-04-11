const remote = require('electron').remote;
const ipcRenderer = require('electron').ipcRenderer;
const fs = require('fs');
const path = require('path');

const {shell} = require('electron');

class FileManager {

	constructor({ editor, monaco }) {
		this.editor = editor;
		this.monaco = monaco;
		this.f = "";

		// When we receive a 'open-file' message, open the file
		ipcRenderer.on('open-file', (e, file) => this.openFile(file));
		ipcRenderer.on('save-file', (e, file) => this.saveFile(file));
	}


	openFile(file) {
		let data = fs.readFileSync(path.resolve(__dirname, file), 'utf-8');
		this.editor.setModel(this.monaco.editor.createModel(data));
	}

	saveAsFile() {
		var filename = remote.dialog.showSaveDialog();

		if(!filename) return;

		const model = this.editor.getModel();
		let data = '';

		model._lines.forEach((line) => {
			data += line.text + model._EOL;
		});

		fs.writeFile(filename.toString(), data, 'utf-8', function(err) {
			if(err) {
				throw err;
			}
		});
	}

	saveFile(file) {
		if(!file) {
			this.saveAsFile();
			console.log("file is " + this.f)
			return
		}

		const model = this.editor.getModel();
		let data = '';

		model._lines.forEach((line) => {
			data += line.text + model._EOL;
		});

		fs.writeFile(file, data, 'utf-8', function(err) {
			if(err) {
				throw err;
			}
		});
	}

	readFolder(path) {

		if(path === null || path === undefined) { 
			if (process.platform == "win32") {
				path = 'C:/'
			} else if (process.platform == "linux") {
				path = '/'
			}
		}

		console.log(path)

		fs.readdir(path, (err, files) => {
			'use strict';
			if (err) throw  err;
			document.getElementById('listed-files').innerHTML = `<ol id="display-files"></ol>`;
			for (let file of files) {
				fs.stat(path + file, (err, stats) => {
					let theID = `${path}${file}/`;
					if (err) throw err;
					if (stats.isDirectory()) {
						document.getElementById('display-files').innerHTML += `<li id=${theID} ondblclick="manage.readFolder(this.id)"><i class="far fa-folder-open"></i> ${file}</li>`;
					} else {
						theID = theID.slice(0, -1);
						document.getElementById('display-files').innerHTML += `<li id=${theID} ondblclick="manage.openFile(this.id)"><i class="far fa-file"></i> ${file}</li>`;
					}
				});
			}
		});
	}
}

module.exports = FileManager;