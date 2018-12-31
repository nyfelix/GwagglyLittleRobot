import 'bootstrap';
import $ from 'jquery';
import './scss/app.scss';


var app = function() {
	let HOST = 'http://192.168.1.8';

	$('.sound').on('click', (e) => { 
		$.get(HOST + '/voice/play?track=' + e.target.name)
	});
}();
