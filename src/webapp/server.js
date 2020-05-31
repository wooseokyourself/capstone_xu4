/*
200518 Leekyungmin
HGU 20-2 capstone web
*/

//1-1) 상태 변수
var mode = 1;		// 1~3.admin / 4.basic
var isSet = false;
var isON_child = false;
var MAX_CAMERA = 4;
//1-2) 상수
var express_port = 10051;
var root_path = __dirname + '/../..';
var path_child = root_path + '/bin/a.out'
var path_img_ROI = 'config/images/';
var path_src_ROI = root_path + '/config/images/';
var path_img_deap = 'resources/images/';
var path_src_deap = root_path + '/resources/images/';
var path_data_ROI = root_path + '/config/ROI.txt';
var path_data_people = root_path + '/resources/people.txt';
var path_data_camera = root_path + '/resources/camera_ip.txt';
var path_data_admin = root_path + '/config/admin_input.txt'
var path_data_mode = root_path + '/config/mode.txt'
//1-3) 일반 변수
var num_camera;
var res_capture=new Array(2);;
var res_resize=new Array(2);
var client_num = 0;
var client_log = 0;
var curr_image_name = "";
var curr_image_camera = 0;
var curr_image_num = -1;
var setting_bnum = new Array();;
var setting_plist = new Array();;
var num_people = 0;
var num_src_deap = new Array();
var num_src_ROI = 0;
var num_curr_ROI = 1;
var list_src_deap = new Array(MAX_CAMERA);
var list_people = new Array(MAX_CAMERA);
for(var i =0 ; i<MAX_CAMERA; i++){
	list_src_deap[i] = new Array();
	list_people[i] = new Array();
}
var list_src_ROI;
var list_data_people = new Array();
var list_data_camera = new Array();
//1-4) 일반 함수
function findNumber(list, filename){
	// list - 0:전체 / 홀수:파일명 / 짝수:사람수
	for(var i =0 ; i<list.length; i++){
		if(list[i]==filename){
			return list[i+1];
		}
	}
	return -1;
}

//2-1) 모듈 동작 변수 선언 (express, ejs, fs)
var express = require('express');
var ejs = require('ejs');
var app = express();
var fs = require('fs');
//2-2) mode 생성
fs.writeFileSync(path_data_mode, "admin\n", 'utf8');
//2-3) child 실행
var spawn = require('child_process').spawn;
var child;
//2-4) 엔진 설정 (view engine - ejs + static path 설정 + express listen)
app.set("view engine", "ejs");
app.set('views', __dirname+'/views');
app.use(express.static(__dirname+"/../.."));
app.listen(express_port, function(){
	console.log('Express Start, Port: ' + express_port);
});

//3) web root page 설정
app.get('/', function(req, res){
	//3-1) Waiting
	switch(mode){
		case 1:
			res.render('admin_1', {
				isSet: isSet,
				c_num: MAX_CAMERA
			});
			break;
		case 2:
			if(!isON_child){
				console.log("child procoss is now on\n");
				child = spawn(path_child, [root_path]);
				var scriptOutput = "";
				child.stdout.setEncoding('utf8');
				child.stdout.on('data', function(data) {
					console.log('stdout: ' + data);
					data = data.toString();
					scriptOutput += data;
				});

				child.stderr.setEncoding('utf8');
				child.stderr.on('data', function(data) {
					console.log('stderr: ' + data);
					data = data.toString();
					scriptOutput += data;
				});

				child.on('close', function(code) {
					console.log('closing code: ' + code);
					console.log('Full output of script: ', scriptOutput);
				})

				isON_child = true;
			}
			list_src_ROI = fs.readdirSync(path_src_ROI);
			num_src_ROI = list_src_ROI.length;
			if(!num_src_ROI){
				res.render('admin_wait');
			}
			else{
				mode = 3;
				res.redirect('/');
			}
			break;
		case 3:
			res.render('admin_2', {
				path: path_img_ROI,
				img_list: list_src_ROI,
				num_curr: num_curr_ROI,
				num_img: num_camera
				// num_img:num_src_ROI
			});
			break;
		case 4:
			list_data_people = fs.readFileSync(path_data_people, 'utf8').toString().split("\n");
			list_data_camera = fs.readFileSync(path_data_camera, 'utf8').toString().split("\n");
			num_people = 0;
			for(var i = 0; i<num_camera; i++){
				list_src_deap[i] = fs.readdirSync(path_src_deap+String(i+1));
				for(var j = 0; j<list_src_deap[i].length; j++){
					var tmp = findNumber(list_data_people, list_src_deap[i][j]);
					list_people[i][j] = tmp;
				}
				num_src_deap[i] = list_src_deap[i].length;
				num_people += parseInt(list_people[i][list_src_deap[i].length-1]);
			}
			res.render('basic', {
				path: path_img_deap,					//이미지 경로
				p_num: num_people,						//총 사람 수
				p_list: list_people,					//[][] - i번째 카메라의 j번째 사진의 사람 수
				img_list: list_src_deap,				//[][] - i번째 카메라의 j번째 사진이름
				c_num: num_camera,						//카메라의 숫자
				c_log: num_src_deap,					//[] - i번째 카메라의 사진 수
				c_ip: list_data_camera,					//[] - i번째 카메라의 ip
				curr_img_name: curr_image_name,			//현재 선택한 이미지의 이름
				curr_img_camera: curr_image_camera,		//현재 선택한 이미지의 카메라 넘버
				curr_img_num: curr_image_num,			//현재 선택한 이미지의 인원수
			});
			break;
	}
});

//4) REST API: data using URL
//4-1) admin_1
app.get('/ADMIN_1/toADMIN_wait', function(req, res){
	num_camera=req.query.num_cam;
	res_capture[0]=req.query.res_cw;
	res_capture[1]=req.query.res_ch;
	res_resize[0]=req.query.res_rw;
	res_resize[1]=req.query.res_rh;
	mode=2;
	fs.writeFileSync(path_data_admin, num_camera+"\n"+res_capture[0]+"\n"+res_capture[1]+"\n"+res_resize[0]+"\n"+res_resize[1]+"\n", 'utf8');
	fs.writeFileSync(path_data_mode, "admin\n", 'utf8');
	res.redirect('/');
})
app.get('/ADMIN_1/toBASIC', function(req, res){
	console.log("admin_1 to basic");
	mode = 4;
	res.redirect('/');
})
//4-2) admin_2
app.get('/ADMIN_2/next', function(req, res){
	setting_bnum[num_curr_ROI-1] = req.query.b_num;
	setting_plist[num_curr_ROI-1] = JSON.parse(req.query.p_list);
	num_curr_ROI++;
	res.redirect('/');
});
app.get('/ADMIN_2/prev', function(req, res){
	num_curr_ROI--;
	res.redirect('/');
});
app.get('/ADMIN_2/toBASIC', function(req, res){
	console.log("admin_2 to basic");
	setting_bnum[num_curr_ROI-1] = req.query.b_num;
	setting_plist[num_curr_ROI-1] = JSON.parse(req.query.p_list);
	fs.writeFileSync(path_data_ROI, "", 'utf8');
	fs.writeFileSync(path_data_mode, "basic\n", 'utf8');
	for(var i =0 ; i< num_camera; i++){
		for(var j =0; j<setting_bnum[i]; j++){
			for(var k=0; k<4; k++){
				fs.appendFileSync(path_data_ROI, setting_plist[i][j][k], 'utf8');
				fs.appendFileSync(path_data_ROI, ' ', 'utf8');
			}
		}
		fs.appendFileSync(path_data_ROI, '\n', 'utf8');
	}
	num_curr_ROI = 1;
	mode=4;
	isSet = true;
	res.redirect('/');
});
//4-3) basic
app.get('/BASIC/image/:camera/:filepath', function(req, res){
	curr_image_camera = req.params.camera;
	curr_image_name = req.params.filepath;
	curr_image_num = findNumber(list_data_people, curr_image_name);
	res.redirect('/');
});
app.get('/BASIC/toADMIN_1', function(req, res){
	console.log("basic to admin_1");
	mode = 1;
	res.redirect('/');
})
