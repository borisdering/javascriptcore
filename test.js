console.log('this is a log message');
console.info('this is a info message');
console.warn('this is a warn message');
console.error('this is a error message');

setInterval(() => { console.log('interval 300 milis.'); }, 300);

let buffer0 = Buffer.alloc(8);
var buffer1 = Buffer.from([ 8, 6, 7, 5, 3, 0, 9]);
var buffer2 = Buffer.from("I'm a string!", "utf-8");
buffer0.readUInt16();
console.log(buffer0.toString())

fetch("http://www.googlemde")