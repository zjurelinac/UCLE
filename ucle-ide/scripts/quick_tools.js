const { remote, ipcRenderer } = require('electron');

const Menu = remote.Menu;
const MenuItem = remote.MenuItem;
var simRunning = false;
var line = 1;

var registers = ["IIF","PC","R0","R1","R2","R3","R4","R5","R6","R7","SP","SR"];

module.exports = (editor, fileManager, ucleTabs, ucleServer) => {

	document.getElementById("listed-files").addEventListener("click", function(e) {
		if (e.target && (e.target.matches("li.dir") || e.target.matches("div.dir"))) {
			var child = e.target.children[0]; 
			if(child.className == "dir-ico-opened") {
				child.className = "dir-ico-closed";
			} else {
				child.className = "dir-ico-opened";
			}
			fileManager.readFolder(e.target.id, false);
		}
	});

	function addButtonClick(button) {
		button.addEventListener("click", function(e) {
			fileManager.openDirectory();
		});
	}

	function calculateWidth(running) {
		var tools = document.getElementById("quick-tools");
		var style = window.getComputedStyle(tools , null);

		if(style.display == "none") {
			if(running) {
				document.getElementById("show-sim").classList.remove("hide");			
				document.getElementById("show-sim").style.width = "21%";
				document.getElementById("container").style.width = "78%";
			} else {
				document.getElementById("show-sim").className = "hide";			
				document.getElementById("container").style.width = "100%";
			}
		} else {
			if(running) {
				document.getElementById("show-sim").classList.remove("hide");			
				document.getElementById("show-sim").style.width = "18%";
				document.getElementById("container").style.width = "60%";
			} else {
				document.getElementById("show-sim").className = "hide";			
				document.getElementById("container").style.width = "78%";
			}
		}
	}

	function addSimEvents() {
		document.getElementById("step").className = "run-simulation";
		document.getElementById("continue").className = "run-simulation";
		document.getElementById("rm-breakpoints").className = "run-simulation";

		document.getElementById("step").addEventListener("click", stepSim);
		document.getElementById("continue").addEventListener("click", continueSim);
		document.getElementById("rm-breakpoints").addEventListener("click", function(e) { console.log ("rm")});
	}

	editor.onDidChangeModelDecorations(function(e) {
		if(!simRunning) return;
		var model = editor.getModel();
	});

	function removeSimEvents() {
		document.getElementById("step").className = "wait-simulation";
		document.getElementById("continue").className = "wait-simulation";
		document.getElementById("rm-breakpoints").className = "wait-simulation";

		document.getElementById("step").removeEventListener("click", stepSim);
		document.getElementById("continue").removeEventListener("click", continueSim);
		document.getElementById("rm-breakpoints").removeEventListener("click", function(e) { console.log ("rm")});
	}

	function clearRegisterInfo() {
		var registerTable = document.getElementById("registers").rows;
		
		for(var i = 1; i <= registers.length; i++) {
			var row = registerTable[i];
			var regName = row.cells[0];
			var regValue = row.cells[1];
			if(regName.innerHTML == "IIF") {
				regValue.innerHTML = "false";
			} else {
				regValue.innerHTML = "0";
			}
		}
	}

	function stepSim() {
		var model = ucleTabs.currentTabModel;
		line = ucleServer.findFirstNonEmpty(model, line);
		if(line == -1) {
			ucleServer.removeHighLight(model);
			document.getElementById("step").className = "wait-simulation";
			document.getElementById("step").removeEventListener("click", stepSim);
			ucleServer.sendCommand("step",[]);
			//document.getElementById("continue").className = "wait-simulation";
			//document.getElementById("continue").removeEventListener("click", continueSim);

			return;
		}
		ucleServer.addHighLight(model, new monaco.Range(line,1,line,1));
		ucleServer.sendCommand("step",[]);
	}

	function continueSim() {
		var model = ucleTabs.currentTabModel;
		line = ucleServer.findFirstBreakPoint(model, line);

		if(line == -1) {
			ucleServer.removeHighLight(model);

			document.getElementById("step").className = "wait-simulation";
			document.getElementById("step").removeEventListener("click", stepSim);
			document.getElementById("continue").className = "wait-simulation";
			document.getElementById("continue").removeEventListener("click", continueSim);
		} else {
			ucleServer.addHighLight(model, new monaco.Range(line,1,line,1));
		}

		ucleServer.sendCommand("cont",[]);
	}

	document.getElementById("listed-files").addEventListener("contextmenu", function(e) {
		if (e.target && (e.target.matches("li.dir") || e.target.matches("div.dir"))) {
			var contextMenu = new Menu();

			contextMenu.append(new MenuItem({
				click() {
					fileManager.removeFolder(e.target.id); 
					if(!document.querySelector('[id^="div-"')) {
						document.getElementById('workspace-text').style.display = "inline-block";
						var button = document.getElementById("openbtn");
						button.style.display = "block";
						addButtonClick(button);
					}
				},
				label: "Remove folder from workspace"
			}));

			contextMenu.popup(remote.getCurrentWindow());
		}
	});

	document.getElementById("listed-files").addEventListener("dblclick", function(e) {
		if(e.target && e.target.matches("li.file")) {
			fileManager.openFile(e.target.id);
			ucleTabs.addTab({title: ucleTabs.getFileName(e.target.id), fullPath: e.target.id},true);
		}
	});

	document.getElementById("listed-files").addEventListener("mouseover", function(e) {
		e.preventDefault();
		if(e.target && (e.target.matches("li.dir") || e.target.matches("li.file") || e.target.matches("div.dir"))) {
			document.getElementById(e.target.id).style.cursor = "pointer";
			document.getElementById(e.target.id).style.background = "#e9e9e9"
		}
	});

	document.getElementById("listed-files").addEventListener("mouseout", function(e) {
		e.preventDefault();
		if(e.target && (e.target.matches("li.dir") || e.target.matches("li.file") || e.target.matches("div.dir"))) {
			document.getElementById(e.target.id).style.background = "white";
		}
	});

	document.getElementById("file-explorer").addEventListener("click", function(e) {
		var tools = document.getElementById("quick-tools");
		var style = window.getComputedStyle(tools , null);
		var sim = document.getElementById("show-sim");
		console.log(sim.style.width);

		editor.focus();

		if(style.display == "none") {
			tools.style.display = "inline-block";
			tools.style.width = "20%";
		} else {
			tools.style.display = "none";
			tools.style.width = "0%";
		}

		calculateWidth(simRunning);
	});

	document.getElementById("run-sim").addEventListener("click", function(e) {
		if(e.target.className == "run-simulation") {
			var currTabValue = ucleTabs.currentTabValue;
			if(currTabValue) {
				ucleTabs.hideTabs();
				e.target.className = "stop-simulation";

				addSimEvents();
				calculateWidth(true);

				simRunning = true;
				ucleTabs.simRunning= true;
				fileManager.simRunning = true;

				ucleServer.registerBreakPoints(ucleTabs.currentTabModel);
				ucleServer.addHighLight(ucleTabs.currentTabModel, new monaco.Range(1,1,1,1));

				var filePath = ucleTabs.currentTab.querySelector('.ucle-tab-file-path').textContent;

				editor.setPosition(new monaco.Position(1,1));

				var data = ucleServer.runSim(filePath);
			} else {
				var type = "warning";
				var buttons = ['OK'];
				var message = 'Cannot run simulation with an empty file!';
				var defaultId = 0;
				var response = remote.dialog.showMessageBox({message, type, buttons, defaultId});
			}
		} else {
			e.target.className = "run-simulation";
			removeSimEvents();
			calculateWidth(false);

			simRunning = false;
			ucleTabs.simRunning= false;
			fileManager.simRunning = false;

			line = 1;

			clearRegisterInfo();
			ucleServer.stopSim(ucleTabs.currentTabModel);
			ucleServer.removeHighLight(ucleTabs.currentTabModel);

			ucleTabs.showAllTabs();
		}	
	});

	ipcRenderer.on('open-dir', (e) => {
		fileManager.openDirectory();
	});

	document.getElementById("workspace").addEventListener("click", function(e) {
		var workspace = document.getElementById('workspace');
		var listedFiles = document.getElementById('listed-files');
		var style = window.getComputedStyle(listedFiles , null);

		if(workspace.children[0].className == "explorer-ico-opened") {
			workspace.children[0].className = "explorer-ico-closed";
			listedFiles.style.display = "none";
		} else {
			workspace.children[0].className = "explorer-ico-opened";
			listedFiles.style.display = "inherit";			
		}
	});

	document.getElementById("explorer").addEventListener("click", function(e) {
		var explorer = document.getElementById('explorer');

		if(explorer.children[0].className == "explorer-ico-opened") {
			explorer.children[0].className = "explorer-ico-closed";
		} else {
			explorer.children[0].className = "explorer-ico-opened";
		}
	});

	ipcRenderer.on('run-simulation', (e) => {
		var sim = document.getElementById("run-sim");
		if(sim.className == "run-simulation") {
			var currTabValue = ucleTabs.currentTabValue;
			if(currTabValue) {
				ucleTabs.hideTabs();
				e.target.className = "stop-simulation";

				addSimEvents();
				calculateWidth(true);

				simRunning = true;
				ucleTabs.simRunning= true;
				fileManager.simRunning = true;

				ucleServer.registerBreakPoints(ucleTabs.currentTabModel);
				ucleServer.addHighLight(ucleTabs.currentTabModel, new monaco.Range(1,1,1,1));

				var filePath = ucleTabs.currentTab.querySelector('.ucle-tab-file-path').textContent;

				editor.setPosition(new monaco.Position(1,1));

				var data = ucleServer.runSim(filePath);
			} else {
				var type = "warning";
				var buttons = ['OK'];
				var message = 'Cannot run simulation with an empty file!';
				var defaultId = 0;
				var response = remote.dialog.showMessageBox({message, type, buttons, defaultId});
			}
		} else {
			e.target.className = "run-simulation";
			removeSimEvents();
			calculateWidth(false);

			simRunning = false;
			ucleTabs.simRunning= false;
			fileManager.simRunning = false;

			line = 1;

			clearRegisterInfo();
			ucleServer.stopSim(ucleTabs.currentTabModel);
			ucleServer.removeHighLight(ucleTabs.currentTabModel);

			ucleTabs.showAllTabs();
		}	
	});

	ipcRenderer.on('sim-response', (e, data) => {
		var json = JSON.parse(data);

		if(json.type == "register_info") {
			var registerJSON = Object.keys(json.registers).map(function(key) {
				return [String(key), json.registers[key]];
			});
			var registerTable = document.getElementById("registers").rows;
			
			for(var i = 1; i <= registers.length; i++) {
				var row = registerTable[i];
				var regValue = row.cells[1];

				var highlight = regValue.style.backgroundColor;

				if(regValue.innerHTML != registerJSON[i-1][1].toString()) {
					regValue.style.backgroundColor = "#bdbdbd";
					regValue.innerHTML = registerJSON[i-1][1].toString();
					setTimeout(function() {		
						for(var i = 1; i <= registers.length; i++) {
							var row = registerTable[i];
							var regValue = row.cells[1];
							if(regValue.style.backgroundColor == "rgb(189, 189, 189)") {
								regValue.style.backgroundColor = "initial";
							}
						}
					}, 1000);
				}
			}
		}
	});

	addButtonClick(document.getElementById("openbtn"));
	document.getElementById("show-sim").className = "hide";

	var regTable = document.getElementById("registers");

	for(var i = 1; i <= registers.length; i++) {
		var row = regTable.insertRow(i);

		var regName = row.insertCell(0);
		var regValue = row.insertCell(1);

		regName.innerHTML = registers[i-1];

		if(registers[i-1] == "IIF") {
			regValue.innerHTML = "false";
		} else {
			regValue.innerHTML = "0";
		}
	}
};