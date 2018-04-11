// Save Monaco's amd require and restore Node's require
var amdRequire = global.require;

var path = require('path');
function uriFromPath(_path) {
	var pathName = path.resolve(_path).replace(/\\/g, '/');
	if (pathName.length > 0 && pathName.charAt(0) !== '/') {
		pathName = '/' + pathName;
	}
	return encodeURI('file://' + pathName);
}
amdRequire.config({
	baseUrl: uriFromPath(path.join(__dirname, 'node_modules/monaco-editor/min'))
});
// workaround monaco-css not understanding the environment
self.module = undefined;
// workaround monaco-typescript not understanding the environment
self.process.browser = true;
amdRequire(['vs/editor/editor.main'], function() {
	var editor = monaco.editor.create(document.getElementById('container'), {
		theme: "light",
		contextmenu: false,
		quickSuggestions: false,
		automaticLayout: true
	});
});