/**
 * An example of how2 bind a class 'MyClass' to Squirrel.
 *
 * This demo exposes the class, its methods and a signal.
 *
 */


class MyClass : Object 
{
    public MyClass() {
        stdout.printf("Constructor\n");
    }

    ~MyClass() {
        stdout.printf("Destructor\n");
    }

    public int foo() {
        print("native foo\n");
        return 9001; 
    }

    public void trigger_thing()
    {
        on_thing();
    }

    public signal void on_thing();
}


/**
 * MyClassWrap keeps a record of squirrel closures used as callback functions
 * The destructor releases all the callbacks from the VM.
 * This is neccessary to prevent memory leaks when classes are instantiated and destroyed by Squirrel
 */
class MyClassWrap : Object
{
    public MyClassWrap(Squirrel.Vm v) {
        callbacks = new SList<Squirrel.Obj>(); 
        vm = v;
    }

    ~MyClassWrap() {
        for(int i = 0; i < callbacks.length(); i++) {
            Squirrel.Obj o = callbacks.nth_data(i);
            vm.release(o);
        }
    }

    public SList<Squirrel.Obj> callbacks;
    private Squirrel.Vm vm;
}


public int main(string[] args) 
{
    var vm = new Squirrel.Vm(1024);
    vm.on_print.connect((vm, str) => {
        print(str + "\n");
        stdout.flush();
    });

    // create a class named MyClass in the root table
    vm.push_root_table();
    vm.push_string("MyClass");
    vm.new_class(false);


    // create the constructor
    // the constructor is used to create an instance of the class
    vm.push_string("constructor");
    vm.new_closure((vm) => {
        // create the actual Vala object
        MyClassWrap mc = new MyClassWrap(vm);
        // a class instance in Squirrel can hold a 'user pointer'
        // use the user pointer to store a reference to the Vala object
        vm.set_instance_up(-1, mc);
        // ref because we just added a reference to the squirrel instance
        mc.ref();

        // the release hook is called when the class instance is destroyed
        vm.set_release_hook(-1, (ptr, sz) => {
            // we hook it to unref the Vala object
            // this will destroy the Vala object
            MyClassWrap m = ptr as MyClassWrap;
            m.unref();
            return 0; 
        });
        // return 1 because this returns 1 new instance of the class
        return 1;
    }, 0);
    vm.new_slot(-3, false); // add the constructor to the class


    // create a function called 'connect' to connect signals back to squirrel
    vm.push_string("connect");
    vm.new_closure((vm) => {
        // get the pointer to the Vala object from the user pointer of this class instance
        MyClassWrap* gg;
        vm.get_instance_up(1, out gg, null, false);

        string signal_name;
        vm.get_string(-2, out signal_name); // signal name is passed as the 'second last' parameter


        Squirrel.Obj callback;
        vm.get_stack_object(-1, out callback); //get the callback closure as a Squirrel Object
        vm.add_ref(callback); // reference it so the VM doesn't destroy it as it goes out of scope

        gg->callbacks.append(callback); // add to the list of callbacks - so it can be unreferenced later

        Squirrel.Obj self; // keep a copy of the class instance
        vm.get_stack_object(-3, out self);

        switch(signal_name) {
            case "on_thing": // on thing signal
                gg->on_thing.connect(() => {
                    vm.push_object(callback); // push the closure
                    vm.push_object(self); // push 'this'
                    vm.call(1, true, true);  // call the function

                    // if(vm.get_object_type(-1) == Squirrel.OBJECTTYPE.INTEGER) 
                    // { 
                    //     int i;
                    //     vm.get_int(-1, out i); 
                    //     return i; 
                    // }
                    // else { return vm.throw_error("invalid return type, expected int"); }
                });
                break;
            default:
                return vm.throw_error("no such signal: " + signal_name);
        }

        return 0; // no values returned
    }, 0);
    vm.new_slot(-3, false); //put the 'connect' function into the class


    vm.push_string("foo"); // wrap the foo function
    vm.new_closure((vm) => {
        MyClassWrap* gg;
        vm.get_instance_up(1, out gg, null, false);
        int result = gg->foo();
        vm.push_int(result);
        return 1;
    }, 0);
    vm.new_slot(-3, false);

    // adds a value to the class
    // to read/write this value in C, use the get/set functions
    // to get,
    // 1. push the key e.g. vm.push_string("num");
    // 2. vm.get(idx); where idx is the index of the class instance
    // 3. the value is at the top of the stack, use vm.get_int() to retrieve it

    // to set, 
    // 1. push the value e.g. vm.push_int(100); 
    // 2. push the key e.g. vm.push_string("num");
    // 3. vm.set(idx); where idx is the stack index of the class instance to set the key to this value

    // in a member function call, the idx of the class instance is always 1
    vm.push_string("num");
    vm.push_int(0);
    vm.new_slot(-3, false);


    // create a member function to trigger the signal
    vm.push_string("trigger_thing");
    vm.new_closure((vm) => {
        MyClass* gg;
        vm.get_instance_up(1, out gg, null, false);
        gg->trigger_thing();
        return 0;
    }, 0);
    vm.new_slot(-3, false);

    // add the class to the root table
    // note is just a definition for a class, not an instance
    // an instance of the class will be created when the Squirrel script calls MyClass();
    vm.new_slot(-3, true);

    vm.do_file("test.nut", true, true);
    return 0;
}

