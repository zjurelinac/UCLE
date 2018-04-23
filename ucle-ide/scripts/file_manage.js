const remote = require('electron').remote;
const ipcRenderer = require('electron').ipcRenderer;
const fs = require('fs');
const path = require('path');
const mime = require('mime');
const dirTree = require('directory-tree');

class FileManager {

	constructor({ editor, monaco }) {
		this.editor = editor;
		this.monaco = monaco;
		this.startPath = './';

		ipcRenderer.on('open-file', (e, file) => this.openFile(file));
		ipcRenderer.on('save-as-file', (e) => this.saveAsFile());
	}

	getFileName(str) {
		return str.split('\\').pop().split('/').pop();
	}

	getDirName(str) {
		return str.match(/([^\/]*)\/*$/)[1];
	}

	openFile(file) {
		let data = fs.readFileSync(file, 'utf-8');
		this.editor.setModel(this.monaco.editor.createModel(data));
	}

	openDirectory() {
		var filename = remote.dialog.showOpenDialog({ properties: ['openDirectory']});
		if(!filename) return;
		this.readFolder(filename.toString() + '/');
	}

	saveAsFile() {
		var filename = remote.dialog.showSaveDialog();

		if(!filename) return;

		const model = this.editor.getModel();

		fs.writeFile(filename.toString(), model.getValue(), 'utf-8', function(err) {
			if(err) {
				throw err;
			}
		});

		return filename;
	}

	saveFile(file) {
		const model = this.editor.getModel();
		fs.writeFile(file, model.getValue(), 'utf-8', function(err) {
			if(err) {
				throw err;
			}
		});
	}

	walkThroughFiles(children, listID) {
		let list = document.getElementById(listID);
		for (var i = 0, len = children.length; i < len; i++) {
			var child = children[i];
			if(child.type == 'directory') {
				let childListID = child.path + '-' + child.name;
				list.innerHTML += '<li id="' + child.path + '" class="dir">' + child.name + '</li><ul id="' + childListID + '"></ul><br/>';
				list.style.display = 'inherit';
			} else {
				list.innerHTML += '<li id="' + child.path + '" class="file">'+ child.name + '</li> <br/>';
			}
		}
	}

	readFolder(filePath, init = true) {
		if(filePath === null || filePath === undefined) { 
			filePath = './'
		}

		this.startPath = filePath;

		var dirName = this.getDirName(filePath);

		let listID = filePath + '-' + dirName;

		if(document.getElementById(listID) != null && document.getElementById(listID).innerHTML != '') {
			if(document.getElementById(listID).style.display == 'none') {
				document.getElementById(listID).style.display = 'inherit';
			} else {
				document.getElementById(listID).style.display = 'none';
			}
			return;
		}
		const tree = dirTree(filePath);

		tree.path = dirName;

		if(init) {
			document.getElementById('openbtn').style.display = "none";

			document.getElementById('listed-files').innerHTML += '<span id="' + filePath + '">' + dirName + '</span><ul id="' + listID + '"></ul>';

			document.getElementById(filePath).addEventListener('click', function(e) {
					if(document.getElementById(listID).style.display == "none") {
						document.getElementById(listID).style.display = "inherit";
					} else {
						document.getElementById(listID).style.display = "none";
					}
			});

			document.getElementById(filePath).addEventListener('mouseover', function(e) {
					document.getElementById(filePath).style.cursor = "pointer";
			});

		}

		this.walkThroughFiles(tree.children, listID);
	}
}

module.exports = FileManager;