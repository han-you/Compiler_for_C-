#include <vector>
#include <iostream>
#include <fstream>
using namespace std;
#define ID 1
#define NUM 2
#define OTHER_TYPE 3
#define RESERVED_WORD 4
class Token
{
public:
  string name;
  string val;
  int linenum;
  Token(string name, string val, int linepos) : name(name), val(val), linenum(linepos){};
};
string SourcePath = "test.txt";
fstream InFile(SourcePath, ios::in);
vector<string> V;
vector<Token> V2;
void print(string &token, int pos, int type, bool flag) // flag 代表有没有错
{
  if (flag)
  {
    if (type == ID)
    {
      if (token == "else" || token == "if" || token == "return" || token == "void" || token == "int" || token == "while")
      {
        V2.push_back(Token(token, "", pos));
        cout << "      " << pos << " : reserved word :" << token << endl;
      }
      else
      {
        V2.push_back(Token("ID", token, pos));
        cout << "      " << pos << " : ID    name= " << token << endl;
      }
    }
    else if (type == NUM)
    {
      V2.push_back(Token("NUM", token, pos));
      cout << "      " << pos << " : NUM    val= " << token << endl;
    }
    else
    {
      V2.push_back(Token(token, "", pos));
      cout << "      " << pos << " : " << token << endl;
    }
  }
  else
  {
    cout << "      " << pos << " : ERROR   " << token << endl;
    exit(-1);
  }
}

bool isAlpha(char tmp)
{
  return (tmp >= 'A' && tmp <= 'Z') || (tmp >= 'a' && tmp <= 'z');
}
bool isDigit(char tmp)
{
  return tmp >= '0' && tmp <= '9';
}
bool preHandle()
{
  vector<string> V2;
  int left = 0;
  bool incomment = false;
  int maxLeftCount = 0;
  int rightCount = 0;

  for (auto s : V)
  {
    string scpy = "";
    bool HasComment = false; // 防止刚出incomment被判断可以加入V
    for (int i = 0; i < s.size(); i++)
    {
      scpy += s[i];
      if (s[i] == '/' && i + 1 < s.size() && s[i + 1] == '*')
      {
        left++;
        maxLeftCount++;
        HasComment = true;
        incomment = true;
      }
      else if (s[i] == '*' && i + 1 < s.size() && s[i + 1] == '/')
      {
        rightCount++;
        if (maxLeftCount > 1 && rightCount >= 2)
          return false;
        if (left > 0)
          HasComment = true;
        left--;
        incomment = false;
      }
      else if (s[i] == ' ')
      {
        while (i < s.size() && s[i] == ' ')
          i++;
        i--;
      }
    }
    // if (scpy.size() > 2 && scpy[scpy.size() - 1] != '/' && scpy[scpy.size() - 2] != '*')
    //   continue;
    if (HasComment)
    {
      V2.push_back("");
      continue;
    }
    if (!incomment)
      V2.push_back(scpy);
    else
      V2.push_back("");
  }
  V = V2;
  if (rightCount != maxLeftCount)
  {
    cout << "注释错误" << endl;
    exit(-1);
  }
  return !incomment;
}

int state;
void getToken()
{
  int i = 0;
  for (auto s : V)
  {
    i++;
    if (s == "")
      continue;
    cout << i << " : " << s << endl;
    int pos = 0;
    char ch;
    string token;
    while (pos != s.size())
    {
      token = "";
      state = 0;
      ch = s[pos];
      while (state != 100)
      {
        switch (state)
        {
        case 0:
          if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == ',' || ch == ';' || ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == '[' || ch == ']')
            state = 1;
          else if (ch == '<')
            state = 2;
          else if (ch == '>')
            state = 3;
          else if (ch == '=')
            state = 4;
          else if (ch == '!')
            state = 5;
          else if (isAlpha(ch))
            state = 6;
          else if (isDigit(ch))
            state = 9;
          else if (ch == ' ')
            state = 8;
          else
            state = 7;
          break;
        case 1:
          token += ch;
          print(token, i, OTHER_TYPE, true);
          state = 100;
          break;
        case 2: //<
          token += ch;
          pos++;
          ch = s[pos];
          if (ch == '=')
          {
            state = 10;
            break;
          }
          else
          {
            state = 11;
            break;
          }

        case 3: //>
          token += ch;
          pos++;
          ch = s[pos];
          if (ch == '=')
          {
            state = 12;
            break;
          }
          else
          {
            state = 13;
            break;
          }

        case 4: //=
          token += ch;
          pos++;
          ch = s[pos];
          if (ch == '=')
          {
            state = 14;
            break;
          }
          else
          {
            state = 15;
            break;
          }

        case 5: //!
          token += ch;
          pos++;
          ch = s[pos];
          if (ch == '=')
          {
            state = 16;
            break;
          }
          else
          {
            state = 17;
            break;
          }

        case 6: // 字母
          token += ch;
          pos++;
          ch = s[pos];
          if (isAlpha(ch))
          {
            state = 6;
            break;
          }
          else if (isDigit(ch))
          {
            state = 18;
            break;
          }
          else
          {
            state = 20;
            break;
          }

        case 7: // 其他字符
          token += ch;
          print(token, i, NUM, false);
          state = 100;
          break;
        case 8: // 空格子
          state = 100;
          break;
        case 9: // 数字
          token += ch;
          pos++;
          ch = s[pos];
          if (isAlpha(ch))
          {
            state = 21;
            break;
          }
          else if (isDigit(ch))
          {
            state = 9;
            break;
          }
          else
          {
            state = 23;
            break;
          }
        case 10: //<= end
          token += ch;
          print(token, i, OTHER_TYPE, true);
          state = 100;
          break;
        case 11: //< end
          pos--;
          print(token, i, OTHER_TYPE, true);
          state = 100;
          break;
        case 12: //>=end
          token += ch;
          print(token, i, OTHER_TYPE, true);
          state = 100;
          break;
        case 13: //>
          pos--;
          print(token, i, OTHER_TYPE, true);
          state = 100;
          break;
        case 14: //==
          token += ch;
          print(token, i, OTHER_TYPE, true);
          state = 100;
          break;
        case 15: //=
          pos--;
          print(token, i, OTHER_TYPE, true);
          state = 100;
          break;
        case 16: //!=
          token += ch;
          print(token, i, OTHER_TYPE, true);
          state = 100;
          break;
        case 17: //!
          pos--;
          print(token, i, OTHER_TYPE, false);
          state = 100;
          break;
        case 18:
          token += ch;
          pos++;
          ch = s[pos];
          if (isAlpha(ch) || isDigit(ch))
          {
            state = 18;
            break;
          }
          else
          {
            state = 19;
            break;
          }
        case 19:
          pos--;
          print(token, i, OTHER_TYPE, false);
          state = 100;
          break;
        case 20:
          pos--;
          print(token, i, ID, true);
          state = 100;
          break;
        case 21:
          token += ch;
          pos++;
          ch = s[pos];
          if (isAlpha(ch) || isDigit(ch))
          {
            state = 21;
            break;
          }
          else
          {
            state = 22;
            break;
          }
        case 22:
          pos--;
          print(token, i, OTHER_TYPE, false);
          state = 100;
          break;
        case 23:
          pos--;
          print(token, i, NUM, true);
          state = 100;
          break;
        }
      }
      pos++;
      token = "";
    }
  }
}

// 语法分析
// const `1
// Op 2
// Id 3
// If 4
// specifier 5
// Assign To 6
// 空 7
enum class type
{
  program_type,
  declaration_list_type,
  declaration_type,
  var_declaration_type,
  var_declaration2_type,
  type_specifier_type,
  fun_declaration_type,
  params_type,
  param_list_type,
  param_type,
  compound_stmt_type,
  local_declarations_type,
  statement_list_type,
  statement_type,
  expression_stmt_type,
  selection_stmt_type,
  iteration_stmt_type,
  return_stmt_type,
  return_stmt2_type,
  expression_type,
  var_type,
  simple_expression_type,
  relop_type,
  additive_expression_type,
  addop_type,
  term_type,
  mulop_type,
  factor_type,
  call_type,
  args_type,
  arg_list_type,
  ID_type,
  const_type,
  Op_type,
  Else_type,
  If_type,
  Specifier_type,
  Assigned_to_type,
  // bracket_type,
  While_type,
  Return_type,
};
class Node
{
public:
  type NodeType;
  vector<Node *> childs;
  string val;
  bool judge;
  Node() : val(""){};
  Node(type nodetype) : NodeType(nodetype), val(""){};
};
int curpos = 0;
Node *program();
Node *declaration_list();
Node *declaration();
Node *var_declaration();
Node *var_declaration2();
Node *type_specifier();
Node *fun_declaration();
Node *params();
Node *param_list();
Node *param();
Node *compound_stmt();
Node *local_declarations();
Node *statement_list();
Node *statement();
Node *expression_stmt();
Node *selection_stmt();
Node *iteration_stmt();
Node *return_stmt();
Node *return_stmt2();
Node *expression();
Node *var();
Node *simple_expression();
Node *relop();
Node *additive_expression();
Node *addop();
Node *term();
Node *mulop();
Node *factor();
Node *call();
Node *args();
Node *arg_list();
Node *getNode(type nodetype)
{
  return new Node(nodetype);
}
Node *getNode()
{
  return new Node();
}
// Node *getRacketNode()
// {
//   Node *node;
//   node = getNode(type::bracket_type);
//   node->val = V2[curpos - 1].name;
//   return node;
// }
Node *getIDNode()
{
  Node *node;
  node = getNode(type::ID_type);
  node->val = V2[curpos - 1].val;
  return node;
}
Node *getNumNode()
{
  Node *node;
  node = getNode(type::const_type);
  node->val = V2[curpos - 1].val;
  return node;
}
Node *getSpecifierNode(string s)
{
  Node *node = getNode(type::Specifier_type);
  node->val = s;
  return node;
}
Node *getIfNode()
{
  Node *node = getNode(type::If_type);
  node->val = "if";
  return node;
}
Node *getElseNode()
{
  Node *node = getNode(type::Else_type);
  node->val = "else";
  return node;
}
Node *getWhileNode()
{
  Node *node = getNode(type::While_type);
  node->val = "while";
  return node;
}
Node *getReturnNode()
{
  Node *node = getNode(type::Return_type);
  node->val = "return";
  return node;
}

Node *getOpNode(string s)
{
  Node *node = getNode(type::Op_type);
  node->val = s;
  return node;
}
void error()
{
  cout << "语法分析错误:" << " 位于行:" << V2[curpos - 1].linenum << "  ";
  if (V2[curpos - 1].name == "ID" || V2[curpos - 1].name == "NUM")
    cout << V2[curpos - 1].name << "  " << V2[curpos - 1].val;
  else
    cout << V2[curpos - 1].name;
  cout << "附近";
  cout << endl;
  exit(-1);
}
bool matchToken(string expected)
{

  if (curpos != V2.size())
  {
    if (V2[curpos].name == expected)
    {
      curpos++;
      return true;
    }
    else
      return false;
  }
  else
    return false;
}

Node *program()
{
  Node *ans = getNode(type::program_type);
  Node *node = declaration_list();
  if (node->judge)
  {
    ans->childs.push_back(node);
    ans->judge = true;
    return ans;
  }
  ans->judge = false;
  return ans;
}

Node *declaration_list()
{
  Node *ans = getNode(type::declaration_list_type);
  Node *node = declaration();
  if (!node->judge)
  {
    ans->judge = false;
    return ans;
  }
  ans->childs.push_back(node);
  while (1)
  {
    node = declaration();
    bool tmp = node->judge;
    if (!tmp)
    {
      break;
    }
    ans->childs.push_back(node);
  }
  ans->judge = true;
  return ans;
}
Node *declaration()
{
  Node *ans = getNode(type::declaration_type);
  int pos = curpos;
  Node *node;

  node = fun_declaration();
  if (node->judge)
  {
    ans->judge = true;
    ans->childs.push_back(node);
    return ans;
  }
  curpos = pos;
  node = var_declaration();
  if (node->judge)
  {
    ans->judge = true;
    ans->childs.push_back(node);
    return ans;
  }
  ans->judge = false;
  return ans;
}

Node *var_declaration()
{
  Node *ans = getNode(type::var_declaration_type);
  Node *node = type_specifier();
  if (node->judge)
  {
    ans->childs.push_back(node);
    if (matchToken("ID"))
    {
      ans->childs.push_back(getIDNode());
      node = var_declaration2();
      if (node->judge)
      {
        ans->judge = true;
        ans->childs.push_back(node);
        return ans;
      }
    }
    error();
  }
  ans->judge = false;
  return ans;
}

Node *var_declaration2()
{
  Node *ans = getNode(type::var_declaration2_type);
  if (curpos == V2.size())
    error();
  if (matchToken(";"))
  {
    ans->judge = true;
    return ans;
  }
  else
  {
    if (matchToken("["))
    {
      if (matchToken("NUM"))
      {
        ans->childs.push_back(getNumNode());
        if (matchToken("]"))
        {
          if (matchToken(";"))
          {
            ans->judge = true;
            return ans;
          }
        }
      }
      error();
    }
    ans->judge = false;
    return ans;
  }
}

Node *type_specifier()
{
  Node *ans = getNode(type::type_specifier_type);
  if (matchToken("int"))
  {
    ans->judge = true;
    ans->childs.push_back(getSpecifierNode("int"));
    return ans;
  }
  else if (matchToken("void"))
  {
    ans->judge = true;
    ans->childs.push_back(getSpecifierNode("void"));
    return ans;
  }
  else
  {
    ans->judge = false;
    return ans;
  }
}

Node *fun_declaration()
{
  Node *ans = getNode(type::fun_declaration_type);
  Node *node;
  int pos = curpos;
  node = type_specifier();
  if (node->judge)
  {
    ans->childs.push_back(node);
    if (matchToken("ID"))
    {
      ans->childs.push_back(getIDNode());
      if (matchToken("("))
      {
        node = params();
        if (node->judge)
        {
          ans->childs.push_back(node);
          if (matchToken(")"))
          {
            node = compound_stmt();
            if (node->judge)
            {
              ans->judge = true;
              ans->childs.push_back(node);
              return ans;
            }
          }
        }
      }
    }
  }
  ans->judge = false;
  return ans;
}

Node *params()
{
  Node *ans = getNode(type::params_type);
  Node *node;
  int pos = curpos;
  node = param_list();
  if (node->judge)
  {
    ans->judge = true;
    ans->childs.push_back(node);
    return ans;
  }
  curpos = pos;
  if (matchToken("void"))
  {
    ans->judge = true;
    ans->childs.push_back(getSpecifierNode("void"));
    return ans;
  }
  else
  {
    ans->judge = false;
    return ans;
  }
}

Node *param_list()
{
  Node *ans = getNode(type::param_list_type);
  Node *node = param();
  if (!node->judge)
  {
    ans->judge = false;
    return ans;
  }
  ans->childs.push_back(node);
  while (matchToken(","))
  {
    node = param();
    if (!node->judge)
    {
      ans->judge = false;
      return ans;
    }
    ans->childs.push_back(node);
  }
  ans->judge = true;
  return ans;
}

// Node *param()
// {
//   Node *ans = getNode(type::param_type);
//   Node *node = type_specifier();
//   if (!node->judge)
//   {
//     ans->judge = false;
//     return ans;
//   }
//   ans->childs.push_back(node);
//   if (!matchToken("ID"))
//   {
//     ans->judge = false;
//     return ans;
//   }
//   ans->childs.push_back(getIDNode());
//   if (matchToken("["))
//   {
//     ans->childs.push_back(getRacketNode());
//     if (matchToken("]"))
//     {
//       ans->childs.push_back(getRacketNode());
//       ans->judge = true;
//       return ans;
//     }
//     else
//     {
//       ans->judge = false;
//       return ans;
//     }
//   }
//   ans->judge = true;
//   return ans;
// }
Node *param()
{
  Node *ans = getNode(type::param_type);
  Node *node = type_specifier();
  if (node->judge)
  {
    ans->childs.push_back(node);
    if (matchToken("ID"))
    {
      ans->childs.push_back(getIDNode());
      if (matchToken("["))
      {
        // ans->childs.push_back(getRacketNode());
        if (matchToken("]"))
        {
          // ans->childs.push_back(getRacketNode());
          ans->judge = true;
          return ans;
        }
        error();
      }
      ans->judge = true;
      return ans;
    }
  }
  ans->judge = true;
  return ans;
}
Node *compound_stmt()
{
  Node *ans = getNode(type::compound_stmt_type);
  Node *node;
  if (matchToken("{"))
  {
    node = local_declarations();
    if (node->judge)
    {
      ans->childs.push_back(node);
      node = statement_list();
      if (node->judge)
      {
        ans->childs.push_back(node);
        if (matchToken("}"))
        {
          ans->judge = true;
          return ans;
        }
      }
    }
    error();
  }
  ans->judge = false;
  return ans;
}

Node *local_declarations()
{
  Node *ans = getNode(type::local_declarations_type);
  Node *node;
  while (1)
  {
    node = var_declaration();
    if (!node->judge)
      break;
    ans->childs.push_back(node);
  }
  ans->judge = true;
  return ans;
}

Node *statement_list()
{
  Node *ans = getNode(type::statement_list_type);
  Node *node;
  while (1)
  {
    node = statement();
    if (!node->judge)
      break;
    ans->childs.push_back(node);
  }
  ans->judge = true;
  return ans;
}

Node *statement()
{
  Node *ans = getNode(type::statement_type);
  Node *node;
  int pos = curpos;
  node = expression_stmt();
  if (node->judge)
  {
    ans->childs.push_back(node);
    ans->judge = true;
    return ans;
  }
  curpos = pos;
  node = compound_stmt();
  if (node->judge)
  {
    ans->judge = true;
    ans->childs.push_back(node);
    return ans;
  }
  curpos = pos;
  node = selection_stmt();
  if (node->judge)
  {
    ans->judge = true;
    ans->childs.push_back(node);
    return ans;
  }
  curpos = pos;
  node = iteration_stmt();
  if (node->judge)
  {
    ans->judge = true;
    ans->childs.push_back(node);
    return ans;
  }
  curpos = pos;
  node = return_stmt();
  if (node->judge)
  {
    ans->judge = true;
    ans->childs.push_back(node);
    return ans;
  }

  ans->judge = false;
  return ans;
}

Node *expression_stmt()
{
  Node *ans = getNode(type::expression_stmt_type);
  Node *node;
  int pos = curpos;
  node = expression();
  if (node->judge)
  {
    ans->childs.push_back(node);
    if (curpos == V2.size())
      error();
    if (matchToken(";"))
    {
      ans->judge = true;
      return ans;
    }
    error();
  }
  curpos = pos;
  if (matchToken(";"))
  {
    ans->judge = true;
    return ans;
  }

  ans->judge = false;
  return ans;
}

// Node *selection_stmt()
// {
//   Node *ans = getNode(type::selection_stmt_type);
//   Node *node;
//   if (!matchToken("if"))
//   {
//     ans->judge = false;
//     return ans;
//   }
//   ans->childs.push_back(getIfNode());
//   if (!matchToken("("))
//   {
//     ans->judge = false;
//     return ans;
//   }
//   node = expression();
//   if (!node->judge)
//   {
//     ans->judge = false;
//     return ans;
//   }
//   ans->childs.push_back(node);
//   if (!matchToken(")"))
//   {
//     ans->judge = false;
//     return ans;
//   }
//   ans->childs.push_back(getRacketNode());
//   node = statement();
//   if (!node->judge)
//   {
//     ans->judge = false;
//     return ans;
//   }
//   ans->childs.push_back(node);
//   if (matchToken("else"))
//   {
//     ans->childs.push_back(getElseNode());
//     node = statement();
//     if (node->judge)
//     {
//       ans->childs.push_back(node);
//       ans->judge = true;
//       return ans;
//     }
//     else
//     {
//       ans->judge = false;
//       return ans;
//     }
//   }

//   ans->judge = true;
//   return ans;
// }
Node *selection_stmt()
{
  Node *ans = getNode(type::selection_stmt_type);
  Node *node;
  if (matchToken("if"))
  {
    ans->childs.push_back(getIfNode());
    if (matchToken("("))
    {
      node = expression();
      if (node->judge)
      {
        ans->childs.push_back(node);
        if (matchToken(")"))
        {
          node = statement();
          if (node->judge)
          {
            ans->childs.push_back(node);
            if (matchToken("else"))
            {
              ans->childs.push_back(getElseNode());
              node = statement();
              if (node->judge)
              {
                ans->childs.push_back(node);
                ans->judge = true;
                return ans;
              }
              error();
            }
            ans->judge = true;
            return ans;
          }
        }
      }
    }
    error();
  }
  ans->judge = false;
  return ans;
}
Node *iteration_stmt()
{
  Node *ans = getNode(type::iteration_stmt_type);
  Node *node;
  if (!matchToken("while"))
  {
    ans->judge = false;
    return ans;
  }
  ans->childs.push_back(getWhileNode());
  if (!matchToken("("))
  {
    ans->judge = false;
    return ans;
  }
  node = expression();
  if (!node->judge)
  {
    ans->judge = false;
    return ans;
  }
  ans->childs.push_back(node);
  if (!matchToken(")"))
  {
    ans->judge = false;
    return ans;
  }
  node = statement();
  if (!node->judge)
  {
    ans->judge = false;
    return ans;
  }
  ans->childs.push_back(node);
  ans->judge = true;
  return ans;
}

Node *return_stmt()
{
  Node *ans = getNode(type::return_stmt_type);
  Node *node;
  if (!matchToken("return"))
  {
    ans->judge = false;
    return ans;
  }
  ans->childs.push_back(getReturnNode());
  node = return_stmt2();
  if (!node->judge)
  {
    ans->judge = false;
    return ans;
  }
  ans->childs.push_back(node);
  ans->judge = true;
  return ans;
}

Node *return_stmt2()
{
  Node *ans = getNode(type::return_stmt2_type);
  Node *node;
  int pos = curpos;
  node = expression();
  if (node->judge)
  {
    ans->childs.push_back(node);
    if (matchToken(";"))
    {
      ans->judge = true;
      return ans;
    }
    error();
  }
  curpos = pos;
  if (matchToken(";"))
  {
    ans->judge = true;
    return ans;
  }

  ans->judge = false;
  return ans;
}

Node *expression()
{
  Node *ans = getNode(type::expression_type);
  Node *node;
  int pos = curpos;

  node = var();
  if (node->judge)
  {
    ans->NodeType = type::Assigned_to_type;
    ans->childs.push_back(node);
    if (matchToken("="))
    {
      ans->childs.push_back(getOpNode("="));
      node = expression();
      if (node->judge)
      {
        ans->childs.push_back(node);
        ans->judge = true;
        return ans;
      }
    }
  }
  ans->NodeType = type::expression_type;
  curpos = pos;
  for (int i = 0; i < ans->childs.size(); i++)
  {
    delete ans->childs[i];
  }
  ans->childs = vector<Node *>();
  node = simple_expression();
  if (node->judge)
  {
    ans->childs.push_back(node);
    ans->judge = true;
    return ans;
  }
  ans->judge = false;
  return ans;
}

Node *var()
{
  Node *ans = getNode(type::var_type);
  Node *node;
  if (!matchToken("ID"))
  {
    ans->judge = false;
    return ans;
  }
  ans->childs.push_back(getIDNode());
  if (matchToken("["))
  {
    node = expression();
    if (node->judge)
    {
      ans->childs.push_back(node);
      if (matchToken("]"))
      {
        ans->judge = true;
        return ans;
      }
    }

    ans->judge = false;
    return ans;
  }
  ans->judge = true;
  return ans;
}

Node *simple_expression()
{
  Node *ans = getNode(type::simple_expression_type);
  Node *node;
  node = additive_expression();
  if (!node->judge)
  {
    ans->judge = false;
    return ans;
  }
  ans->childs.push_back(node);
  node = relop();
  if (node->judge)
  {
    ans->childs.push_back(node);
    node = additive_expression();
    if (node->judge)
    {
      ans->childs.push_back(node);
      ans->judge = true;
      return ans;
    }

    ans->judge = false;
    return ans;
  }

  ans->judge = true;
  return ans;
}

Node *relop()
{
  Node *ans = getNode(type::relop_type);
  Node *node;
  if (matchToken("<="))
  {
    ans->childs.push_back(getOpNode("<="));
    ans->judge = true;
    return ans;
  }
  else if (matchToken(">="))
  {
    ans->childs.push_back(getOpNode(">="));
    ans->judge = true;
    return ans;
  }
  else if (matchToken("=="))
  {
    ans->childs.push_back(getOpNode("=="));
    ans->judge = true;
    return ans;
  }
  else if (matchToken("!="))
  {
    ans->childs.push_back(getOpNode("!="));
    ans->judge = true;
    return ans;
  }
  else if (matchToken(">"))
  {
    ans->childs.push_back(getOpNode(">"));
    ans->judge = true;
    return ans;
  }
  else if (matchToken("<"))
  {
    ans->childs.push_back(getOpNode("<"));
    ans->judge = true;
    return ans;
  }
  else
  {
    ans->judge = false;
    return ans;
  }
}

Node *additive_expression()
{
  Node *ans = getNode(type::additive_expression_type);
  Node *node;
  node = term();
  if (!node->judge)
  {
    ans->judge = false;
    return ans;
  }
  ans->childs.push_back(node);
  while (1)
  {
    node = addop();
    if (!node->judge)
      break;
    ans->childs.push_back(node);
    node = term();
    if (!node->judge)
    {
      ans->judge = false;
      return ans;
    }
    ans->childs.push_back(node);
  }
  ans->judge = true;
  return ans;
}
Node *addop()
{
  Node *ans = getNode(type::addop_type);
  Node *node;
  if (matchToken("+"))
  {
    ans->childs.push_back(getOpNode("+"));
    ans->judge = true;
    return ans;
  }
  else if (matchToken("-"))
  {
    ans->childs.push_back(getOpNode("-"));
    ans->judge = true;
    return ans;
  }

  ans->judge = false;
  return ans;
}
Node *term()
{
  Node *ans = getNode(type::term_type);
  Node *node;
  node = factor();
  if (!node->judge)
  {
    ans->judge = false;
    return ans;
  }
  ans->childs.push_back(node);
  while (1)
  {
    node = mulop();
    if (!node->judge)
      break;
    ans->childs.push_back(node);
    node = factor();
    if (!node->judge)
    {
      ans->judge = false;
      return ans;
    }
    ans->childs.push_back(node);
  }
  ans->judge = true;
  return ans;
}

Node *mulop()
{
  Node *ans = getNode(type::mulop_type);
  Node *node;
  if (matchToken("*"))
  {
    ans->childs.push_back(getOpNode("*"));
    ans->judge = true;
    return ans;
  }
  else if (matchToken("/"))
  {
    ans->childs.push_back(getOpNode("/"));
    ans->judge = true;
    return ans;
  }
  ans->judge = false;
  return ans;
}

Node *factor()
{
  Node *ans = getNode(type::factor_type);
  Node *node;
  int pos = curpos;
  if (matchToken("("))
  {
    node = expression();
    if (node->judge)
    {
      ans->childs.push_back(node);
      if (matchToken(")"))
      {
        ans->judge = true;
        return ans;
      }
    }
    error();
  }
  curpos = pos;
  node = call();
  if (node->judge)
  {
    ans->childs.push_back(node);
    ans->judge = true;
    return ans;
  }
  curpos = pos;
  node = var();
  if (node->judge)
  {
    ans->childs.push_back(node);
    ans->judge = true;
    return ans;
  }
  curpos = pos;
  if (matchToken("NUM"))
  {
    ans->childs.push_back(getNumNode());
    ans->judge = true;
    return ans;
  }

  ans->judge = false;
  return ans;
}

Node *call()
{
  Node *ans = getNode(type::call_type);
  Node *node;
  if (matchToken("ID"))
  {
    ans->childs.push_back(getIDNode());
    if (matchToken("("))
    {
      node = args();
      if (node->judge)
      {
        ans->childs.push_back(node);
        if (matchToken(")"))
        {
          ans->judge = true;
          return ans;
        }
      }
      error();
    }
  }

  ans->judge = false;
  return ans;
}
Node *args()
{
  Node *ans = getNode(type::args_type);
  Node *node;
  int pos = curpos;
  node = arg_list();
  if (node)
  {
    ans->childs.push_back(node);
    ans->judge = true;
    return ans;
  }
  curpos = pos;

  ans->judge = true;
  return ans;
}

Node *arg_list()
{
  Node *ans = getNode(type::arg_list_type);
  Node *node;
  node = expression();
  if (!node->judge)
  {
    ans->judge = false;
    return ans;
  }
  ans->childs.push_back(node);
  while (matchToken(","))
  {
    node = expression();
    if (!node->judge)
    {
      ans->judge = false;
      return ans;
    }
    ans->childs.push_back(node);
  }
  ans->judge = true;
  return ans;
}

// 将分析树输出成语法树
string NodeTypeToString(Node *root)
{
  if (root == nullptr)
    return "";
  string str;
  switch (root->NodeType)
  {
  case type::program_type:
    str = "program";
    break;
  case type::declaration_list_type:
    str = "declaration_list";
    break;
  case type::declaration_type:
    str = "declaration";
    break;
  case type::var_declaration_type:
    str = "var_declaration";
    break;
  case type::var_declaration2_type:
    str = "var_declaration2";
    break;
  case type::type_specifier_type:
    str = "type_specifier";
    break;
  case type::fun_declaration_type:
    str = "fun_declaration";
    break;
  case type::params_type:
    str = "params";
    break;
  case type::param_list_type:
    str = "param_list";
    break;
  case type::param_type:
    str = "param";
    break;
  case type::compound_stmt_type:
    str = "compound_stmt";
    break;
  case type::local_declarations_type:
    str = "local_declarations";
    break;
  case type::statement_list_type:
    str = "statement_list";
    break;
  case type::statement_type:
    str = "statement";
    break;
  case type::expression_stmt_type:
    str = "expression_stmt";
    break;
  case type::selection_stmt_type:
    str = "selection_stmt";
    break;
  case type::iteration_stmt_type:
    str = "iteration_stmt";
    break;
  case type::return_stmt_type:
    str = "return_stmt";
    break;
  case type::return_stmt2_type:
    str = "return_stmt2";
    break;
  case type::expression_type:
    str = "expression";
    break;
  case type::var_type:
    str = "var";
    break;
  case type::simple_expression_type:
    str = "simple_expression";
    break;
  case type::relop_type:
    str = "relop";
    break;
  case type::additive_expression_type:
    str = "additive_expression";
    break;
  case type::addop_type:
    str = "addop";
    break;
  case type::term_type:
    str = "term";
    break;
  case type::mulop_type:
    str = "mulop";
    break;
  case type::factor_type:
    str = "factor";
    break;
  case type::call_type:
    str = "call";
    break;
  case type::args_type:
    str = "args";
    break;
  case type::arg_list_type:
    str = "arg_list";
    break;
  case type::ID_type:
    str = "ID";
    break;
  case type::const_type:
    str = "const";
    break;
  case type::Op_type:
    str = "Op";
    break;
  case type::Else_type:
    str = "Else";
    break;
  case type::If_type:
    str = "If";
    break;
  case type::Specifier_type:
    str = "Specifier";
    break;
  case type::Assigned_to_type:
    str = "Assigned_to";
    break;
  // case type::bracket_type:
  //   str = "bracket";
  //   break;
  case type::While_type:
    str = "While";
    break;
  case type::Return_type:
    str = "Return";
    break;
  default:
    str = "Unknown";
  }
  return str;
}
void printSpace(int depth)
{
  while (depth--)
  {
    cout << "  ";
  }
}
void printSyntacticTree(Node *root, int depth)
{
  if (root == nullptr)
    return;
  string NodeType = NodeTypeToString(root);
  vector<Node *> childs = root->childs;
  if (NodeType == "compound_stmt")
  {
    printSyntacticTree(childs[0], depth);
    printSyntacticTree(childs[1], depth);
  }
  else if (NodeType == "var_declaration" || NodeType == "fun_declaration")
  {
    int count = 0;
    for (auto child : childs)
    {
      printSyntacticTree(child, depth + count);
      count++;
    }
  }
  else if (NodeType == "param")
  {
    printSyntacticTree(childs[0], depth);
    printSyntacticTree(childs[1], depth + 1);
  }
  else if (NodeType == "type_specifier" || NodeType == "relop" || NodeType == "addop" || NodeType == "mulop" || NodeType == "param_list" || NodeType == "params" || NodeType == "var_declaration2" || NodeType == "declaration" || NodeType == "declaration_list" || NodeType == "program" || NodeType == "args" || NodeType == "arg_list" || NodeType == "factor" || NodeType == "return_stmt2" || NodeType == "return_stmt" || NodeType == "local_declarations" || NodeType == "statement_list" || NodeType == "statement" || NodeType == "expression_stmt")
  {
    for (auto child : childs)
    {
      printSyntacticTree(child, depth);
    }
  }
  else if (NodeType == "selection_stmt")
  {
    printSyntacticTree(childs[0], depth);
    printSyntacticTree(childs[1], depth + 1);
    printSyntacticTree(childs[2], depth + 1);
    if (childs.size() > 3)
    {
      printSyntacticTree(childs[3], depth);
      printSyntacticTree(childs[4], depth + 1);
    }
  }
  else if (NodeType == "iteration_stmt")
  {
    printSyntacticTree(childs[0], depth);
    printSyntacticTree(childs[1], depth + 1);
    printSyntacticTree(childs[2], depth + 1);
  }
  else if (NodeType == "expression")
  {
    printSyntacticTree(childs[0], depth);
  }
  else if (NodeType == "var")
  {
    printSyntacticTree(childs[0], depth);
    if (childs.size() > 1)
    {
      printSyntacticTree(childs[1], depth + 1);
    }
  }
  else if (NodeType == "simple_expression")
  {
    if (childs.size() > 1)
    {
      printSyntacticTree(childs[1], depth);
      printSyntacticTree(childs[0], depth + 1);
      printSyntacticTree(childs[2], depth + 1);
    }
    else
    {
      printSyntacticTree(childs[0], depth);
    }
  }
  else if (NodeType == "additive_expression" || NodeType == "term" || NodeType == "simple_expression")
  {
    if (childs.size() > 1)
    {
      int pos = childs.size() - 2;
      int newdepth = depth;
      while (pos >= 1)
      {
        printSyntacticTree(childs[pos], newdepth);
        newdepth++;
        pos -= 2;
      }
      pos = 0;
      printSyntacticTree(childs[0], newdepth);
      pos += 2;
      while (pos < childs.size())
      {
        printSyntacticTree(childs[pos], newdepth);
        newdepth--;
        pos += 2;
      }
    }
    else
    {
      printSyntacticTree(childs[0], depth);
    }
  }
  else if (NodeType == "call")
  {
    printSyntacticTree(childs[0], depth);
    printSyntacticTree(childs[1], depth + 1);
  }
  else if (NodeType == "Assigned_to")
  {
    printSpace(depth);
    cout << "Assigned to:" << endl;
    printSyntacticTree(childs[0], depth + 1);
    printSyntacticTree(childs[2], depth + 1);
  }
  else if (NodeType == "ID" || NodeType == "const" || NodeType == "Op" || NodeType == "Specifier")
  {
    printSpace(depth);
    cout << NodeType << ":" << root->val << endl;
  }
  else if (NodeType == "If" || NodeType == "Else" || NodeType == "While" || NodeType == "Return")
  {
    printSpace(depth);
    cout << NodeType << endl;
  }
}
// void printTree(Node *root, int depth)
// {
//   if (root == nullptr)
//     return;
//   for (int i = 0; i < depth; i++)
//   {
//     cout << "    ";
//     OutFile << "    ";
//   }
//   string str = NodeTypeToString(root);
//   cout << str << "  :  ";
//   OutFile << str << "  :  ";
//   if (root->val != "")
//   {
//     cout << root->val;
//     OutFile << root->val;
//   }
//   cout << endl;
//   OutFile << endl;
//   for (auto child : root->childs)
//   {
//     printTree(child, depth + 1);
//   }
// }
int main()
{
  string tmp;
  while (getline(InFile, tmp))
  {
    V.push_back(tmp);
  }
  InFile.close();
  if (!preHandle())
  {
    cout << "词法分析错误" << endl;
    return 0;
  }
  if (V.size() == 0)
  {
    cout << "文本为空" << endl;
    exit(-1);
  }
  getToken();
  // for (auto s : V)
  // {
  //   cout << s << endl;
  // }
  Node *a = program();
  if (curpos != V2.size())
    error();
  if (a->judge)
  {
    cout << "解析成功" << endl;
    printSyntacticTree(a, 0);
  }
  return 0;
}