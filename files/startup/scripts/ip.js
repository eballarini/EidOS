/*
EidOS - a small RTOS for PIC microntrollers
Copyright (C) 2022  Emanuele Ballarini

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
