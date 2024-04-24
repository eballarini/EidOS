
//initialize C callback
let text_ptr = ffi('char * filetext (char*)');
let mjs_res_prints = ffi('void mjs_res_prints (char*)');

//execute callback to get JSON from file
let ptr = text_ptr("/config/test.cfg");

//parse JSON
let obj = JSON.parse(ptr);

//print key value
mjs_res_prints("IP addr: " + obj.inet_addr + "\r\n");
mjs_res_prints("IP subnet mask: " + obj.inet_mask + "\r\n");
mjs_res_prints("default gateway: " + obj.gateway + "\r\n");
mjs_res_prints("DNS: " + obj.dns + "\r\n");
mjs_res_prints("Protocol: " + obj.proto + "\r\n");