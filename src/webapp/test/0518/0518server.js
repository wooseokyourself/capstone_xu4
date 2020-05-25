var net_server = require('net');

var net_port = 9090;
var express_port = 7070;

//test variable
// var client_num_tmp = 3;
// var client_log_tmp = 6;

//3 var for client information
// var client_ip = new Array();
var client_num = 0;
var client_log = 0;
var socket = new Array();
//var take_pic = new Boolean(false);

// var server = net_server.createServer(function(client) {
//
// 	    client_num++;
// 	    client_ip[client_num-1] = client.remoteAddress;
// 	    socket[client_num-1] = client;
//
// 	    console.log('[%d] Client connection', client_num);
// 	    console.log('   local = %s:%s', client.localAddress, client.localPort);
// 	    console.log('   remote = %s:%s', client.remoteAddress, client.remotePort);
//
// 	    //client.setTimeout(500);
// 	    client.setEncoding('utf8');
//
// 	    client.on('data', function(data) {
// 		            console.log('Received data from client on port %d: %s', client.remotePort, data.toString());
//
// 		            //writeData(client, 'Sending: ' + data.toString());
// 		            //console.log('  Bytes sent: ' + client.bytesWritten);
// 		        });
//
// 	    client.on('end', function() {
// 		            console.log('Client disconnected');
// 		        });
//
// 	    client.on('error', function(err) {
// 		            console.log('Socket Error: ', JSON.stringify(err));
// 		        });
//
// 	    client.on('timeout', function() {
// 		            console.log('Socket Timed out');
// 		        });
// });
//
// server.listen(net_port, function() {
// 	    //console.log('Server listening: ' + JSON.stringify(server.address()));
// 	    console.log('Server socket listening, Port: ' + net_port);
//
// 	    server.on('close', function(){
// 		            console.log('Server Terminated');
// 		        });
// 	    server.on('error', function(err){
// 		            console.log('Server Error: ', JSON.stringify(err));
// 		        });
// });
//
// function writeData(socket, data){
// 	  var success = socket.write(data);
// 	  if (!success){
// 		      console.log("Client Send Fail");
// 		    }
// }

//---add expreess
var express = require('express');
var ejs = require('ejs');
//var bodyParser = require('body-parser');
var app = express();
var fs = require('fs');		//to get 'source' folder

//클릭한 이미지
var recent_image_name = "";
var recent_image_num = -1;
var curr_image_name = "";
var curr_image_camera = 0;
var curr_image_num = -1;
// var image_list;

var isConnected= false;
var isSetting = true;

var setting_bnum = new Array();;
var setting_plist = new Array();;

var path_src_deap = 'source/';
var path_src_ROI = 'data/src_ROI/';
var path_data_ROI = 'data/ROI.txt';
var path_data_people = 'data/people.txt';
var path_data_camera = 'data/camera.txt';

var num_people = -1;
var num_camera = 0;
var num_src_deap = new Array();
var num_src_ROI = 0;
var num_curr_ROI = 1;
var list_src_deap = new Array(20);
for(var i =0 ; i<20; i++){
	list_src_deap[i] = new Array();
}
var list_src_ROI;
var list_data_people = new Array();
var list_data_camera = new Array();

//view engine을 ejs 파일 형태로 설정
app.set("view engine", "ejs");
app.set('views', __dirname+'/views');
//source 내의 이미지를 제공하기 위함
app.use(express.static(__dirname));


app.listen(express_port, function(){
	console.log('Express Start, Port: ' + express_port);
});

app.get('/', function(req, res){

	num_camera = fs.readdirSync(path_src_deap).length;

	// fs.readdir(path_src_deap, function(err_parent, files_parent) {
	// 	if(err_parent){
	// 		console.log(err_parent);
	// 	}
	// 	num_camera = files_parent.length;
	// 	console.log('number of camera: ' + num_camera);
	// });
	// console.log(files_parent.join('\n'));

	for(var i = 0; i<num_camera; i++){
		list_src_deap[i] = fs.readdirSync(path_src_deap+String(i+1));
		num_src_deap[i] = list_src_deap[i].length;

		// console.log((i+1)+"_"+list_src_deap[i]);
		// fs.readdir(path_src_deap+String(i), function(err_child, files_child) {
		// 	num_src_deap[i] = files_child.length;
		// 	list_src_deap[i] = files_child;
		// 	// console.log(i+"_"+num_src_deap[i]);
  	// 	// console.log(files_child.join('\n'));
		// });

	}

	list_src_ROI = fs.readdirSync(path_src_ROI);
	num_src_ROI = list_src_ROI.length;

	// fs.readdir(path_src_ROI, function(err, files) {
	// 	if(err){
	// 		console.log(err);
	// 	}
	// 	num_src_ROI = files.length;
	// 	list_src_ROI = files;
	// 	console.log('number of ROI data: ' + num_src_ROI);
	// 	// console.log(files.join('\n'));
	// });

	isConnected = (num_src_ROI!=0)? true:false;

	if(!isConnected){
		res.render('waiting');
	}
	else if(!isSetting){
    res.render('setting', {
			path: path_src_ROI,
      img_list: list_src_ROI,
			num_curr: num_curr_ROI,
			num_img: num_src_ROI
    });
	}
	else{
		list_data_people = fs.readFileSync(path_data_people, 'utf8').toString().split("\n");
		num_people = list_data_people[0];
		list_data_camera = fs.readFileSync(path_data_camera, 'utf8').toString().split("\n");
		res.render('main', {
			path: path_src_deap,
			p_num: num_people,
			img_list: list_src_deap,
			c_num: num_camera,
			c_log: num_src_deap,
			c_ip: list_data_camera,
			curr_img_name: curr_image_name,
			curr_img_camera: curr_image_camera,
			curr_img_num: curr_image_num,
			recent_img_name: recent_image_name,
			recent_img_num: recent_image_num
		});
	}

});

app.get('/image/:camera/:filepath', function(req, res){
	curr_image_camera = req.params.camera;
	curr_image_name = req.params.filepath;
	recent_image_name = list_src_deap[curr_image_camera-1][num_src_deap[curr_image_camera-1]-1];
	curr_image_num = findNumber(list_data_people, curr_image_name);
	recent_image_num = findNumber(list_data_people , recent_image_name);
	res.redirect('/');
});

function findNumber(list, filename){
// list - 0:전체 / 홀수:파일명 / 짝수:사람수
	for(var i =0 ; i<list.length; i++){
		if(list[i]==filename){
			// console.log("find!");
			return list[i+1];
		}
	}
	console.log(list.length);
	// console.log("Cant find!");
	return -1;
}

// app.get('/camera', function(req, res){
// 	if(client_num!=0){
// 		console.log("Button is clicked!");
// 		for(var i = 0; i < client_num; i++){
// 			writeData(socket[i], "take a picture!!");
// 		}
// 	}
// 	else{
// 		console.log("There is no client!");
// 	}
// });


app.get('/setting/next', function(req, res){
	console.log("next");
	setting_bnum[num_curr_ROI-1] = req.query.b_num;
	setting_plist[num_curr_ROI-1] = JSON.parse(req.query.p_list);
	// console.log(setting_bnum);
	// console.log(setting_plist);
	num_curr_ROI++;
	res.redirect('/');
});

app.get('/setting/prev', function(req, res){
	console.log("prev");
	setting_bnum[num_curr_ROI-1] = req.query.b_num;
	setting_plist[num_curr_ROI-1] = JSON.parse(req.query.p_list);
	// console.log(setting_bnum);
	// console.log(setting_plist);
	num_curr_ROI--;
	res.redirect('/');
});


app.get('/setting/finish', function(req, res){
	console.log("finish");
	setting_bnum[num_curr_ROI-1] = req.query.b_num;
	setting_plist[num_curr_ROI-1] = JSON.parse(req.query.p_list);
	isSetting=true;
	// fs.writeFile(path_data_ROI, "number of ROI\n"+setting_bnum+"\n\ndata of ROI\n", 'utf8', function(err){
	// 	console.log(String(setting_bnum));
	// })
	fs.writeFileSync(path_data_ROI, "number of ROI\n"+setting_bnum+"\n\ndata of ROI\n", 'utf8');
	for(var i =0 ; i< num_src_ROI; i++){
		// fs.appendFile(path_data_ROI, (i+1) + "_" + setting_plist[i] + '\n', 'utf8', function(err){
		// 	console.log(setting_plist[i]);
		// })
		fs.appendFileSync(path_data_ROI, setting_plist[i] + '\n', 'utf8');
		// console.log("(test)" + i + "_" + setting_plist[i]);
	}
	res.redirect('/');
});
