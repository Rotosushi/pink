fn main()
{
    var a := (0, 0, 0, 0, 0);
    a.0 = 5;
    a.4 = 5;
    var z := a.0 + a.4;
    z;
};