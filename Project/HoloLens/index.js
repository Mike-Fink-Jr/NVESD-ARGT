var express = require('express');
var app = express();

var init_height= -.025;
var init_shift=.15;
var init_forward = .15;

var angles = [1,1,10];
var swich =true;
function updateAngle0()
{
	if(swich)
	{
		angles[0]+=1;
		angles[0] %=80;
		if (angles[0]==60)
			swich=false;
		if (angles[0]==0)
			angles[0]+=1;

		//swich=false;
	}else updateAngle1();

}
function updateAngle1()
{
	
		angles[1]+=1;
		angles[1] %=70;
		if (angles[1]==25)
			swich=true;
		if (angles[1]==0)
			angles[1]+=1;

}
function updateAngle2()
{
	angles[2]+=1;
	angles[2] %=180;

}
var address = '/dev/cu.HoloLens_Sensor-RNI-SPP'	
app.get('/sensor',(req, res)=>{console.log(angles);updateAngle0();res.send((angles[0]+30)+" "+(angles[1]+20)+" "+angles[2]);});

app.get('/params',(req, res)=>{res.send(""+init_height+" "+init_shift+" "+init_forward);});

app.get('/',(req, res)=>{console.log("here");res.send("here");});

app.listen(8080, ()=> console.log('listening @ 8080'));

/*
var SerialPort= require('serialport');

port = new SerialPort(address, function(err){
	if(err){return console.log('error: ', err );}
});

port.on('data', function(data){
	angle = data;
	console.log('Data: ',data);
});
*/