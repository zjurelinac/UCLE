const { remote, ipcRenderer } = require('electron');

const Menu = remote.Menu;
const MenuItem = remote.MenuItem;
var simRunning = false;
var line = 1;
var address = 0;

const minFontSize = 8;
const maxFontSize = 32;

var registers = ["IIF","PC","R0","R1","R2","R3","R4","R5","R6","R7","SP","SR"];
var clickedElement = null;

module.exports = (editor, fileManager, ucleTabs, ucleServer) => {

	function addButtonClick(button) {
		button.addEventListener("click", function(e) {
			if(fileManager.openDirectory()) {
				document.getElementById("quick-tools").style.whiteSpace = "nowrap";
			}
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

	// ---------------------------------------------------------------------------------
	// ---------------------- Handle simulation functions ------------------------------
	// ---------------------------------------------------------------------------------

	function addSimEvents() {
		document.getElementById("step").className = "run-simulation";
		document.getElementById("continue").className = "run-simulation";
		document.getElementById("rm-breakpoints").className = "run-simulation";
		document.getElementById("reset").className = "run-simulation";

		document.getElementById("step").addEventListener("click", stepSim);
		document.getElementById("continue").addEventListener("click", continueSim);
		document.getElementById("rm-breakpoints").addEventListener("click", removeBreakPoints);
		document.getElementById("reset").addEventListener("click", resetProcessor);
	}

	function removeSimEvents() {
		document.getElementById("step").className = "wait-simulation";
		document.getElementById("continue").className = "wait-simulation";
		document.getElementById("rm-breakpoints").className = "wait-simulation";
		document.getElementById("reset").className = "wait-simulation";

		document.getElementById("step").removeEventListener("click", stepSim);
		document.getElementById("continue").removeEventListener("click", continueSim);
		document.getElementById("rm-breakpoints").removeEventListener("click", removeBreakPoints);
		document.getElementById("reset").addEventListener("click", resetProcessor);		
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
		ucleServer.sendCommand("step",[]);
		setTimeout(function() {
			var model = ucleTabs.currentTabModel;
			line = ucleServer.findFirstNonEmpty(model, line, address);
			if(line == -1) {
				ucleServer.removeHighLight(model);
				document.getElementById("step").className = "wait-simulation";
				document.getElementById("step").removeEventListener("click", stepSim);
				document.getElementById("continue").className = "wait-simulation";
				document.getElementById("continue").removeEventListener("click", continueSim);
				return;
			}
			ucleServer.addHighLight(model, new monaco.Range(line,1,line,1));
		}, 100);
	}

	function continueSim() {
		ucleServer.sendCommand("cont",[]);
		setTimeout(function() {
			var model = ucleTabs.currentTabModel;
			line = ucleServer.findFirstBreakPoint(model, line, address);
			if(line == -1) {
				ucleServer.removeHighLight(model);

				document.getElementById("step").className = "wait-simulation";
				document.getElementById("step").removeEventListener("click", stepSim);
				document.getElementById("continue").className = "wait-simulation";
				document.getElementById("continue").removeEventListener("click", continueSim);
			} else {
				ucleServer.addHighLight(model, new monaco.Range(line,1,line,1));
			}
		}, 100);
	}

	function removeBreakPoints() {
		var model = ucleTabs.currentTabModel;
		ucleServer.removeBreakPoints(model);
	}

	function resetProcessor() {
		var model = ucleTabs.currentTabModel;

		ucleServer.sendCommand("reset",[]);
		line = 1;
		address = 0;
		removeSimEvents();
		addSimEvents();
		ucleServer.sendCommand("start",[]);
		ucleServer.resetPosition(model);
	}

	// ---------------------------------------------------------------------------------
	// --------------------------- Handle font size  -----------------------------------
	// ---------------------------------------------------------------------------------

	document.getElementById("increase").addEventListener("click", function(e) {
		var value = document.getElementById("font-size").value;
		if(value == maxFontSize) return;
		document.getElementById("font-size").value = parseInt(value) + 2;
		editor.updateOptions({fontSize: document.getElementById("font-size").value});
	});

	document.getElementById("decrease").addEventListener("click", function(e) {
		var value = document.getElementById("font-size").value;
		if(value == minFontSize) return;
		document.getElementById("font-size").value = parseInt(value) - 2;
		editor.updateOptions({fontSize: document.getElementById("font-size").value});
	});

	document.getElementById("font-size").addEventListener("change", function(e) {
		var value = document.getElementById("font-size").value;
		if((value % 2)) {
			value++;
		}

		if(value >= maxFontSize) {
			value = maxFontSize;
		} else if(value <= minFontSize) {
			value = minFontSize;
		}

		document.getElementById("font-size").value = value;
		editor.updateOptions({fontSize: value});
	});

	// ---------------------------------------------------------------------------------
	// ------------------ Handle showing the files and sim divs ------------------------
	// ---------------------------------------------------------------------------------

	var listedFiles = document.getElementById("listed-files");
	var openedFiles = document.getElementById("opened-files");

	document.getElementById("show-sim").className = "hide";

	addButtonClick(document.getElementById("openbtn"));

	ipcRenderer.on('open-dir', (e) => {
		fileManager.openDirectory();
	});

	window.addEventListener("resize", function(e) {
		console.log(e);
	});

	listedFiles.addEventListener("click", function(e) {
		if (e.target && (e.target.matches("li.dir") || e.target.matches("div.dir"))) {
			var child = e.target.children[0]; 
			if(child.className == "dir-ico-opened") {
				child.className = "dir-ico-closed";
			} else {
				child.className = "dir-ico-opened";
			}
			fileManager.readFolder(e.target.id, false);
		}

		if (e.target && (e.target.matches("li.dir") || e.target.matches("div.dir") || e.target.matches("li.file"))) {
			if(!clickedElement || (clickedElement == e.target)) {
				clickedElement = e.target;
				document.getElementById(e.target.id).style.backgroundColor = "#e6e6e6";
			} else {
				document.getElementById(clickedElement.id).style.backgroundColor = "white";
				clickedElement = e.target;
				document.getElementById(e.target.id).style.backgroundColor = "#e6e6e6";
			}
		}
	});

	listedFiles.addEventListener("contextmenu", function(e) {
		if (e.target && e.target.matches("div.dir")) {
			var contextMenu = new Menu();

			contextMenu.append(new MenuItem({
				click() {
					fileManager.removeFolder(e.target.id); 
					if(!document.querySelector('[id^="div-"')) {
						document.getElementById('workspace-text').style.display = "inline-block";
						var button = document.getElementById("openbtn");
						document.getElementById("quick-tools").style.whiteSpace = "initial";
						button.style.display = "block";
						addButtonClick(button);
					}
				},
				label: "Remove folder from workspace"
			}));

			contextMenu.popup(remote.getCurrentWindow());
		}
	});

	listedFiles.addEventListener("dblclick", function(e) {
		if(e.target && e.target.matches("li.file")) {
			fileManager.openFile(e.target.id);
			ucleTabs.addTab({title: ucleTabs.getFileName(e.target.id), fullPath: e.target.id},true);		
		}
	});

	openedFiles.addEventListener("dblclick", function(e) {
		if (e.target.matches("div.file")) {
			ucleTabs.setCurrentTabByPath(e.target.title);
		}
	});



	openedFiles.addEventListener("contextmenu", function(e) {
		if (e.target.matches("div.file")) {
			var contextMenu = new Menu();

			contextMenu.append(new MenuItem({
				click() {
					ucleTabs.closeTabByPath(e.target.title);
				},
				label: "Close opened file"
			}));

			contextMenu.popup(remote.getCurrentWindow());
		}
	});

	document.getElementById("file-explorer").addEventListener("click", function(e) {
		var tools = document.getElementById("quick-tools");
		var style = window.getComputedStyle(tools , null);
		var sim = document.getElementById("show-sim");

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

	document.getElementById("workspace").addEventListener("click", function(e) {
		var workspace = document.getElementById('workspace');

		if(workspace.children[0].className == "explorer-ico-opened") {
			workspace.children[0].className = "explorer-ico-closed";
			listedFiles.className = "hide";
		} else {
			workspace.children[0].className = "explorer-ico-opened";
			listedFiles.className = "";
		}
	});

	document.getElementById("explorer").addEventListener("click", function(e) {
		if(!document.querySelector('[id^="open-"')) {
			return;
		}

		var explorer = document.getElementById('explorer');

		if(explorer.children[0].className == "explorer-ico-opened") {
			explorer.children[0].className = "explorer-ico-closed";
			openedFiles.className = "hide";
		} else {
			explorer.children[0].className = "explorer-ico-opened";
			openedFiles.className = "";
		}
	});

	// ---------------------------------------------------------------------------------
	// ------------- Handle the simulation start and response --------------------------
	// ---------------------------------------------------------------------------------

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
				editor.updateOptions({readOnly: true});
				var filePath = ucleTabs.currentTab.querySelector('.ucle-tab-file-path').textContent;

				var data = ucleServer.runSim(filePath, ucleTabs.currentTabModel);
				ucleServer.registerBreakPoints(ucleTabs.currentTabModel);
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
			address = 0;

			editor.updateOptions({readOnly: false});
			clearRegisterInfo();
			ucleServer.stopSim(ucleTabs.currentTabModel);
			ucleServer.removeHighLight(ucleTabs.currentTabModel);

			ucleTabs.showAllTabs();
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
				editor.updateOptions({readOnly: true});
				ucleServer.registerBreakPoints(ucleTabs.currentTabModel);
				ucleServer.addHighLight(ucleTabs.currentTabModel, new monaco.Range(1,1,1,1));

				var filePath = ucleTabs.currentTab.querySelector('.ucle-tab-file-path').textContent;

				editor.setPosition(new monaco.Position(1,1));

				var data = ucleServer.runSim(filePath, ucleTabs.currentTabModel);
				ucleServer.registerBreakPoints(ucleTabs.currentTabModel);
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
			address = 0;

			editor.updateOptions({readOnly: false});
			clearRegisterInfo();
			ucleServer.stopSim(ucleTabs.currentTabModel);
			ucleServer.removeHighLight(ucleTabs.currentTabModel);

			ucleTabs.showAllTabs();
		}	
	});

	ipcRenderer.on('sim-response', (e, data) => {
		var dataArray = data.split("\n");

		dataArray.forEach(function(dataElem) {
			if(dataElem == "") return;
			var json = JSON.parse(dataElem);

			if(json.location) {
				address = json.location;
				console.log(address);
			}

			//console.log(json);

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
									return;
								}
							}
						}, 400);
					}
				}
			}
		})
	});

	// ---------------------------------------------------------------------------------
	// ------------------------ Init the register state --------------------------------
	// ---------------------------------------------------------------------------------


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