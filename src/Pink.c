


// main should look something like this for
// the interpretive setup.
int main (int argc, char** argv)
{
  StringInterner*       interned_ids     = CreateStringInterner();
  StringInterner*       interned_ops     = CreateStringInterner();
  SymbolTable*          global_scope     = CreateSymbolTable((SymbolTable*)NULL);
  BinopPrecedenceTable* precedence_table = CreateBinopPrecedenceTable();
  BinopSet*             binops           = CreateBinopSet();
  UnopSet*              unops            = CreateUnopSet();
  Parser*               parser           = CreateParser(interned_ids, interned_ops, precedence_table, binops, unops);
  Environment*          environment      = CreateEnvironemnt(parser, interned_ids, interned_ops, precedence_table, binops, unops);

  RegisterPrimitiveBinops(environment);
  RegisterPrimitiveUnops(environment);

  cout << "Welcome to Pink! v0.0.2\n"
       << "press ctrl+c to exit.\n";

  Repl(stdin, stdout, environment);


  return 0;
}
