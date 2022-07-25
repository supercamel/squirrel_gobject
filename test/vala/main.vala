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

    vm.push_string("hello_world_foo");
    vm.new_closure((vm) => {
        vm.push_string("hello world\n");
        return 1;
    }, 0);
    vm.new_slot(-3, false);

    vm.push_string("native_add");
    vm.new_closure((vm) => {
        var n_args  = vm.get_top();
        if(n_args >= 3) {
            if( (vm.get_object_type(2) == Squirrel.OBJECTTYPE.INTEGER) && 
                (vm.get_object_type(3) == Squirrel.OBJECTTYPE.INTEGER)) 
            {
                long a, b;
                vm.get_int(2, out a);
                vm.get_int(3, out b);
                vm.push_int(a+b);
                return 1;
            }
        }

        vm.throw_error("native_add requires 2 integers");
        return -1;
    }, 0);
    vm.new_slot(-3, false);


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

