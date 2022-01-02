
public static int main() {
    var vm = new Squirrel.Vm(1024);

    vm.on_print.connect((vm, str) => {
        print(str);
    });

    vm.push_root_table();

    vm.push_string("foo");
    vm.new_closure((hvm) => {
        var v = Squirrel.Vm.from_hvm(hvm);
        v.push_string("Hello world\n");
        return 1;
    }, 0);
    vm.new_slot(-3, false);

    vm.do_file("hello.nut", false, true);

    return 0;
}

