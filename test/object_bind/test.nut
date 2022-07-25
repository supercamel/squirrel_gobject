
local c = MyClass();

c.connect("on_thing", function() {
    print("on thing foo");
});

local result = c.foo();
print("c.foo returned " + result.tostring());

c.trigger_thing();
