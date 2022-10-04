fn main()
{
    var a := ((10, 9), (8, 7), (6, 5), (4, 3), (2, 1));
    var b := a.1.1;
    var c := a.2.0;
    var d := b + c;
    d;
};