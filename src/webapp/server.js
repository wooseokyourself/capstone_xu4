/*
200518 Leekyungmin
HGU 20-2 capstone web
*/

//1-1) 상태 변수
var mode = 1;		// 1~3.admin / 4.basic
var isSet = false;
//1-2) 상수
var express_port = 10001;
var path_child = __dirname + '/../../bin/a.out'
var path_img_ROI = 'config/images/';
var path_src_ROI = __dirname + '/../../config/images/';
var path_img_deap = 'resources/images/';
var path_src_deap = __dirname + '/../../resources/images/';
var path_data_ROI = __dirname + '/../../config/ROI.txt';
var path_data_people = __dirname + '/../../resources/people.txt';
var path_data_camera = __dirname + '/../../resources/camera_ip.txt';
var path_data_admin = __dirname + '/../../config/admin_input.txt'
var path_data_mode = __dirname + '/../../config/mode.txt'
//1-3) 일반 변수
var num_camera;
var res_capture=new Array(2);;
var res_resize=new Array(2);
var client_num = 0;
var client_log = 0;
var recent_image_name = "";
var recent_image_num = -1;
var curr_image_name = "";
var curr_image_camera = 0;
var curr_image_num = -1;
var setting_bnum = new Array();;
var setting_plist = new Array();;
var num_people = -1;
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
//1-4) 일반 함수
function findNumber(list, filename){
	// list - 0:전체 / 홀수:파일명 / 짝수:사람수
	for(var i =0 ; i<list.length; i++){
		if(list[i]==filename){
			return list[i+1];
		}
	}
	console.log(list.length);
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
var child = spawn(path_child);
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
				isSet: isSet
			});
			break;
		case 2:
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
			num_people = list_data_people[0];
			list_data_camera = fs.readFileSync(path_data_camera, 'utf8').toString().split("\n");
			for(var i = 0; i<num_camera; i++){
				list_src_deap[i] = fs.readdirSync(path_src_deap+String(i+1));
				num_src_deap[i] = list_src_deap[i].length;
			}
			res.render('basic', {
				path: path_img_deap,
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
	// setting_bnum[num_curr_ROI-1] = req.query.b_num;
	// setting_plist[num_curr_ROI-1] = JSON.parse(req.query.p_list);
	num_curr_ROI--;
	res.redirect('/');
});
app.get('/ADMIN_2/toBASIC', function(req, res){
	console.log("admin_2 to basic");
	setting_bnum[num_curr_ROI-1] = req.query.b_num;
	setting_plist[num_curr_ROI-1] = JSON.parse(req.query.p_list);
	fs.writeFileSync(path_data_ROI, "number of ROI\n"+setting_bnum+"\n\ndata of ROI\n", 'utf8');
	fs.writeFileSync(path_data_mode, "basic\n", 'utf8');
	for(var i =0 ; i< num_camera; i++){
		fs.appendFileSync(path_data_ROI, setting_plist[i] + '\n', 'utf8');
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
	recent_image_name = list_src_deap[curr_image_camera-1][num_src_deap[curr_image_camera-1]-1];
	curr_image_num = findNumber(list_data_people, curr_image_name);
	recent_image_num = findNumber(list_data_people , recent_image_name);
	res.redirect('/');
});
app.get('/BASIC/toADMIN_1', function(req, res){
	console.log("basic to admin_1");
	mode = 1;
	res.redirect('/');
})
