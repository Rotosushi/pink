fn main()
{
    var x := 0;
    var y := 0;
    var a := &x;
    var b := &y;
    *a = 5;
    *b = 9;
    var z := *a + *b;
    z;
}