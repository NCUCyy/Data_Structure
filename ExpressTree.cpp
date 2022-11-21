#include <iostream>
using namespace std;

/*
函数注意：

    string[]类型输入时，每个string元素之间，可用空格分割
    string可直接=赋值，+=追加，==判断是否相等
    
    建议用atoi()、atof()、atod()————【因为：转换失败返回0】
    atoi(str.c_str())：string————>int
    atof(str.c_str())：string————>float
    atod(str.c_str())：string————>double
    不要用stoi()、stof()、stod()【因为：转换失败会抛异常】

    to_string(int)：int——>string
    c_str()：string————>char*
*/

typedef struct BiTnode
{
    string data; // 保存运算符、运算数（string类型：小数、整数、多位整数都可以）
    BiTnode *lchild, *rchild;
} BiTnode, *BiTree;

// 判断是否为运算符
// 是，返回非0；不是，则返回0
bool JudgeOptr(string str)
{
    if (str == "+" || str == "-" || str == "*" || str == "/")
        return true;
    return false;
}
// 判断是否为运算数
// 是，返回非0；不是，则返回0
bool JudgeOpnd(string str)
{
    return atof(str.c_str());
}


/*计算
    传入：一个运算符、两个运算数。返回：运算结果
    注意：opnd1-opnd2、opnd1/opnd2
*/
float Operate(string optr, float opnd1, float opnd2)
{
    switch (optr.c_str()[0])
    {
    case '+':
        return opnd2 + opnd1;
    case '-':
        return opnd1 - opnd2;
    case '*':
        return opnd2 * opnd1;
    case '/':
        return opnd1 / (opnd2 + 0.0);
    default:
        exit(-1);
    }
}
// 计算存储在二叉树中的表达式的值————类似于：后序遍历
float Calculate(BiTree T)
{
    // 正确来说执行不到这句，因为每个节点都是0或2个孩子（即：不存在只有1个孩子的节点；若有，则表达式存储成树时出错）
    if (T == NULL)
    {
        cout << "表达式存储有误！" << endl;
        exit(-1);
    }
    // 如果是运算符
    if (JudgeOptr(T->data))
    {
        float opnd1 = Calculate(T->lchild);
        float opnd2 = Calculate(T->rchild);
        // 注意：opnd1-opnd2、opnd1/opnd2(因为：左子树中的子式在前，右子树中的子式在后)
        return Operate(T->data, opnd1, opnd2);
    }
    // 如果是运算数
    else
    {
        // 则直接返回
        return atof(T->data.c_str());
    }
}
/*
    找到给定子式（start——end）中，最后计算的运算符
    原则：
        1、优先+、-；若没有，则找最后计算的*、/
        2、若子式有括号（但不是首尾直接括号），则一定不选括号内的运算符
            通过：bracket==0判断
            通过：++braket和braket，标记进入/退出括号

    express：完整的表达式
    [start,end]：下标————限定子式的范围
*/
int FindLastOptr(string express[], int start, int end)
{
    int tag = -1;
    // 若子式首尾本身就有括号，就各往里走一步，再开始for遍历
    // 否则for循环一开始，bracket就为1，全遍历完后，才恢复为0
    // ————>第2组if-else语句都进不去（因为：要求bracket为0），无法给tag赋值
    // 无法找到最后计算的运算符
    if (express[start] == "(" && express[end] == ")")
    {
        start++;
        end--;
    }
    // 记录：所在括号的深度
    // ————其实只要知道在括号中，就不会取其中的运算符；知道具体的深度，是为了标记：进入、退出括号
    int bracket = 0;
    // 记录：遍历到当前位置之前，是否已经找到过+或-号
    int prior = 0;

    // 遍历子式
    for (int i = start; i <= end; i++)
    {
        // 如果start-end有括号，则考虑这两步【进入/退出括号】
        if (express[i] == "(")
            // 表示进入括号
            bracket++;
        else if (express[i] == ")")
            // 表示退出括号
            bracket--;

        // 若子式中【有括号】，则不会返回括号内任何运算符（通过上面两步进入/退出括号）
        // 若【不在括号内】，则tag=现在找到的+或-的下标
        if ((express[i] == "+" || express[i] == "-") && bracket == 0)
        {
            // prior标记为1，表示：在当前子式（start-end）中，查找最后计算的运算符时，已经找到过+或-
            prior = 1;
            tag = i;
            //无break, 找【最后一个】符合条件的运算符, 优先选择的+和-（通过prior标记）
        }
        // 若【不在括号内】且【之前还没有找到+或-号，即prior==0】，则tag=现在找到的*或/的下标
        else if ((express[i] == "*" || express[i] == "/") && bracket == 0 && prior == 0)
            tag = i;
        //无break, 找【最后一个】符合条件的运算符
    }
    return tag;
}
void CreateTreeByMidExp(BiTree &T, string express[], int start, int end)
{
    // 子式长度为1————即：只有数字，不用再找最后计算的运算符，而是直接作为叶子节点，并返回
    if (start == end)
    {
        T = new BiTnode;
        T->data = express[start];
        T->lchild = NULL;
        T->rchild = NULL;
        return;
    }
    // 如果不是单个数字，就要找子式（start-end）中最后计算的运算符（在express数组中的下标————记为：tag）
    else
    {
        // 获取：子式【start-end】中最后计算的运算符，在express数组的下标位置
        int tag = FindLastOptr(express, start, end);

        // 没有找到，说明表达式语法错误
        if (tag == -1)
        {
            cout << "表达式语法错误！" << endl;
            exit(-1);
        }
        else
        {
            // 把子式【start-end】中最后计算的运算符，作为当前根节点的值
            T = new BiTnode;
            T->data = express[tag];
        }
        // 若子式直接以括号作为首尾，则start和end直接各往里走一层
        if (express[start] == "(" && express[end] == ")")
        {
            start++;
            end--;
        }
        /*
            【找到+号，作为根节点】
            把表达式从+号开始分成两半【2个子式，递归运算同上】
            同样在左式、右式中，分别再找到最后计算的运算符，作为当前根节点（+号）的左孩子、右孩子
        */
        // 建立左子树：最后计算的+或-的左边式子（【a+b*c】+）
        CreateTreeByMidExp(T->lchild, express, start, tag - 1);
        // 建立右子树：建立最后计算的+或-的左边式子（+【((d*e)+f)*g】）
        CreateTreeByMidExp(T->rchild, express, tag + 1, end);
    }
}


void Print(BiTree T)
{
    cout << T->data;
}

// 根据optr1和optr2的算符优先级，判断是否需要add括号
// 返回1，表示需要add括号
// 只有一种情况时，需要add括号————————optr1（+、-）的优先级低于optr2（*、/）
bool NeedBracket(string optr1, string optr2)
{
    if (optr1 == "*" || optr1 == "/")
    {
        if (optr2 == "+" || optr2 == "-")
            return 1;
    }
    return 0;
}

// 中缀表达式（注意括号的恢复）
void MidOrderTraverse(BiTree T)
{
    if (T == NULL)
        return;

    // 如果当前根节点为运算数（叶子结点），直接输出即可
    if (JudgeOpnd(T->data))
    {
        Print(T);
    }
    // 如果当前根节点为运算符
    else if (JudgeOptr(T->data))
    {
        // 【左】
        // 与左子树比较的算符优先级
        // root为*/，lchild为+-，则需要add括号
        // 特殊情况：左子树为运算数（NeedBracket()也是返回0，表示不用加括号，也成立）
        if (NeedBracket(T->data, T->lchild->data))
        {
            cout << "(";
            MidOrderTraverse(T->lchild);
            cout << ")";
        }
        // 其他情况，都不需要添加括号
        else
        {
            MidOrderTraverse(T->lchild);
        }

        //【根】
        Print(T);

        //【右】
        // 与右子树比较的算符优先级
        // root为*/，lchild为+-，则需要add括号
        // 特殊情况：右子树为运算数（NeedBracket()也是返回0，表示不用加括号，也成立）
        if (NeedBracket(T->data, T->rchild->data))
        {
            cout << "(";
            MidOrderTraverse(T->rchild);
            cout << ")";
        }
        // 其他情况，都不需要添加括号
        else
        {
            MidOrderTraverse(T->rchild);
        }
    }
}
// 前缀表达式（无需考虑括号）
void PreOrderTraverse(BiTree T)
{
    if (T != NULL)
    {
        Print(T);
        PreOrderTraverse(T->lchild);
        PreOrderTraverse(T->rchild);
    }
}
// 后缀表达式（无需考虑括号）
void PostOrderTraverse(BiTree T)
{
    if (T != NULL)
    {
        PostOrderTraverse(T->lchild);
        PostOrderTraverse(T->rchild);
        Print(T);
    }
}


int main()
{
    string express[100];
    string input;
    int i = 0;
    cout << "请输入表达式（#结尾）：" << endl;
    while (true)
    {
        cin >> input;
        if (input != "#")
            express[i++] = input;
        else
            break;
    }
    
    BiTree T;
    CreateTreeByMidExp(T, express, 0, i - 1);

    cout << "前缀表达式：" << endl;
    PreOrderTraverse(T);
    cout << endl;
    cout << "中缀表达式：" << endl;
    MidOrderTraverse(T);
    cout << endl;
    cout << "后缀表达式：" << endl;
    PostOrderTraverse(T);
    cout << endl;

    cout <<"计算结果为：" << endl
         << Calculate(T) << endl;
}