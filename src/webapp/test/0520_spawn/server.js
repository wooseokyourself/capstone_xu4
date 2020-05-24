var name_child = './test'
var spawn = require('child_process').spawn;
var opts = {
	    stdio: [
		    process.stdin,
		    process.stdout,
		    process.stderr,
		    'pipe', 'pipe'
	    ]
};
var child = spawn(name_child, opts);
console.log("hi");

//var clnt_stdout;
//child.stdio[1].read(clnt_stdout, function() {
//	console.log(clnt_stdout);
//});

//var fork = require('child_process').fork;
//var child = fork(name_child);
//child.send(send_data);
