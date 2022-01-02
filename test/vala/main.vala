/**
 * This demo exposes two functions to the squirrel VM
 * - 'hello_world_foo' which simply returns "Hello world\n" as a string
 * - 'native_add' which takes 2 integer parameters, adds them together and returns them
 *
 * It then runs the script.
 *
 * Then calls the add function from the script.
 *
 */

delegate long SquirrelVmDelegate(void* hvm);

// expose_function 
void expose_function(Squirrel.Vm vm, string name, SquirrelVmDelegate callback) {
    // push the function name
    vm.push_string(name);
    // new_closure using the callback. 0 free variables
    vm.new_closure(callback, 0);
    // new_slot pops te closure and the function name from the stack
    // and creates a new entry in the table or class at the specified index
    // which is -3, or whatever was pushed before calling expose_function
    vm.new_slot(-3, false);
}


public static int main() {
    var vm = new Squirrel.Vm(1024);

    // the on_print signal is called whenever squirrel wants to print something
    vm.on_print.connect((vm, str) => {
        // in this case we just print to stdout
        print(str);
    });

    // push the root table, so the expose_function calls are 
    // exposing to global scope
    vm.push_root_table();
    // squirrel runs native functions by invoking function pointers
    // the parameter that squirrel passes is a pointer to the underlying 
    // squirrel hvm object
    // to get the Squirrel.Vm object, you must pass the hvm to Squirrel.Vm.from_hvm()
    expose_function(vm, "hello_world_foo",  (hvm) => {
        var v = Squirrel.Vm.from_hvm(hvm); // get the Vm object
        v.push_string("Hello world\n"); // push a string onto the stack
        return 1; // the number of returned objects
    });

    expose_function(vm, "native_add", (hvm) => {
        var v = Squirrel.Vm.from_hvm(hvm);
        var n_args  = v.get_top();
        if(n_args >= 3) {
            if( (v.get_object_type(2) == Squirrel.OBJECTTYPE.INTEGER) && 
                (v.get_object_type(3) == Squirrel.OBJECTTYPE.INTEGER)) 
            {
                long a, b;
                v.get_int(2, out a);
                v.get_int(3, out b);
                v.push_int(a+b);
                return 1;
            }
        }

        v.throw_error("native_add requires 2 integers");
        return 0;
    });


    vm.do_file("hello.nut", false, true);


    // HOW TO CALL A GLOBAL FUNCTION IN SQUIRREL

    // call the add function
    vm.push_root_table(); // push the root table
    vm.push_string("add"); // push the function name
    // 
    // now the stack has root table which is holding global objects at position -2
    // the string "add" is on top of the stack at position -1
    // get(-2) here will pop the string off the stack, then perform a get operation
    // on the object at position -2 on the stack (which is the root table)
    // so, it's 'getting' the object called 'add' from the root table
    vm.get(-2); 

    // now we set up the stack for the function call
    // first push root table to expose global variables
    vm.push_root_table();
    // now push the parameters
    vm.push_int(5);
    vm.push_int(6);
    // now call the function
    // call will pop all the parameters off the stack
    // we set the retval parameter to true so the VM will store the returned
    // value on the stack
    vm.call(3, true, true);

    // get the return value
    long i;
    vm.get_int(-1, out i);
    // pop the int and the root table off the stack
    vm.pop(2);

    print(i.to_string() + "\n");

    return 0;
}

