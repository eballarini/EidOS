
//initialize C callback
let text_ptr = ffi('char * filetext (char*)');
let mjs_res_prints = ffi('void mjs_res_prints (char*)');

//IO C callbacks
let mjs_pwm_set_dc = ffi('void mjs_pwm_set_dc (int, int)');
let mjs_pwm_set_period = ffi('void mjs_pwm_set_period (int, int)');
let mjs_pwm_on = ffi('void mjs_pwm_on (int)');
let mjs_pwm_off = ffi('void mjs_pwm_off (int)');

let mjs_gpio_set = ffi('void mjs_gpio_set(int)');
let mjs_gpio_unset = ffi('void mjs_gpio_unset(int)');
let mjs_gpio_flip = ffi('void mjs_gpio_flip(int)');
let mjs_delay = ffi('void mjs_delay(int)');

let mjs_temp_less_than = ffi('int mjs_temp_less_than(int)');
let mjs_temp_greater_than = ffi('int mjs_temp_greater_than(int)');

let mjs_humidity_less_than = ffi('int mjs_humidity_less_than(int)');
let mjs_humidity_than = ffi('int mjs_humidity_greater_than(int)');

let mjs_light_less_than = ffi('int mjs_light_less_than(int)');
let mjs_light_greater_than = ffi('int mjs_light_greater_than(int)');

let mjs_pressure_less_than = ffi('int mjs_pressure_less_than(int)');
let mjs_pressure_greater_than = ffi('int mjs_pressure_greater_than(int)');

//execute callback to get JSON from file
let ptr = text_ptr("/startup/config/loop.json");

//parse JSON
let obj = JSON.parse(ptr);

let execute = "yes";

while(1)
{
for (let i = 0; i < obj.loop.length; i++) {

  //evaluate condition
    
  execute = "no";
    
  if(obj.loop[i].when ==="always") {
   execute = "yes";
   }
   else if(obj.loop[i].when==="temp_less_than")
   {
       if(mjs_temp_less_than(obj.loop[i].eval_value)===1)
       {
           execute = "yes";
       }
   }
   else if(obj.loop[i].when==="temp_greater_than")
   {
       if(mjs_temp_greater_than(obj.loop[i].eval_value)===1)
       {
           execute = "yes";
       }
   }
   else if(obj.loop[i].when==="humidity_less_than")
   {
       if(mjs_humidity_less_than(obj.loop[i].eval_value)===1)
       {
           execute = "yes";
       }
   }
   else if(obj.loop[i].when==="humidity_greater_than")
   {
       if(mjs_humidity_greater_than(obj.loop[i].eval_value)===1)
       {
           execute = "yes";
       }
   }
   else if(obj.loop[i].when==="light_less_than")
   {
       if(mjs_light_less_than(obj.loop[i].eval_value)===1)
       {
           execute = "yes";
       }
   }
   else if(obj.loop[i].when==="light_greater_than")
   {
       if(mjs_light_greater_than(obj.loop[i].eval_value)===1)
       {
           execute = "yes";
       }
   }
   else if(obj.loop[i].when==="pressure_less_than")
   {
       if(mjs_pressure_less_than(obj.loop[i].eval_value)===1)
       {
           execute = "yes";
       }
   }
   else if(obj.loop[i].when==="pressure_greater_than")
   {
       if(mjs_pressure_greater_than(obj.loop[i].eval_value)===1)
       {
           execute = "yes";
       }
   }
    
  //execute action

  if (execute==="yes")
  {
  
  if(obj.loop[i].action ==="pwm_on") {
    mjs_pwm_on(obj.loop[i].arguments[0]);
  }
  else if(obj.loop[i].action ==="pwm_off") {
    mjs_pwm_off(obj.loop[i].arguments[0]);
  }
  else if(obj.loop[i].action ==="pwm_set_dc") {
    mjs_pwm_set_dc(obj.loop[i].arguments[0], obj.loop[i].arguments[1]);
  }
  else if(obj.loop[i].action ==="pwm_set_period") {
    mjs_pwm_set_period(obj.loop[i].arguments[0], obj.loop[i].arguments[1]);
  }
  else if(obj.loop[i].action ==="gpio_set") {
    mjs_pwm_gpio_set(obj.loop[i].arguments[0]);
  }
  else if(obj.loop[i].action ==="gpio_unset") {
    mjs_pwm_gpio_unset(obj.loop[i].arguments[0]);
  }
  else if(obj.loop[i].action ==="gpio_flip") {
    mjs_pwm_gpio_flip(obj.loop[i].arguments[0]);
  }
  else {
    mjs_res_prints("Action not recognized\n");
  }
  
  }
  
  //delay
  mjs_delay(obj.loop[i].delay);

} 
}
