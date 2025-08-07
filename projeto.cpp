#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <set>
#include <algorithm>
#include <cctype>

using namespace std;

// ---------- KMP ----------
vector<int> construirTabelaKMP(const string &padrao)
{
    int m = padrao.length();
    vector<int> lps(m, 0);
    int len = 0, i = 1;
    while (i < m)
    {
        if (padrao[i] == padrao[len])
            lps[i++] = ++len;
        else if (len)
            len = lps[len - 1];
        else
            lps[i++] = 0;
    }
    return lps;
}

void buscaKMP(const string &texto, const string &padrao)
{
    vector<int> lps = construirTabelaKMP(padrao);
    int i = 0, j = 0, n = texto.length(), m = padrao.length();
    while (i < n)
    {
        if (padrao[j] == texto[i])
            i++, j++;
        if (j == m)
        {
            cout << "Ocorrencia encontrada na posicao: " << i - j << endl;
            j = lps[j - 1];
        }
        else if (i < n && padrao[j] != texto[i])
        {
            j ? j = lps[j - 1] : i++;
        }
    }
}

// ---------- WILDCARD ----------
bool combinaComCoringa(const string &texto, const string &padrao, int pos)
{
    if (pos + padrao.size() > texto.size())
        return false;
    for (int i = 0; i < padrao.size(); ++i)
        if (padrao[i] != '*' && padrao[i] != texto[pos + i])
            return false;
    return true;
}

void buscaComCoringa(const string &texto, const string &padrao)
{
    for (int i = 0; i <= texto.size() - padrao.size(); ++i)
        if (combinaComCoringa(texto, padrao, i))
            cout << "Coringa corresponde na posicao: " << i << endl;
}

// ---------- AHO-CORASICK ----------
struct NoTrie
{
    unordered_map<char, NoTrie *> filhos;
    NoTrie *falha = nullptr;
    vector<string> encontrados;
};

NoTrie *construirTrie(const vector<string> &padroes)
{
    NoTrie *raiz = new NoTrie();
    for (auto &palavra : padroes)
    {
        NoTrie *no = raiz;
        for (char c : palavra)
            no = no->filhos[c] ? no->filhos[c] : no->filhos[c] = new NoTrie();
        no->encontrados.push_back(palavra);
    }

    queue<NoTrie *> fila;
    for (auto &[c, filho] : raiz->filhos)
    {
        filho->falha = raiz;
        fila.push(filho);
    }

    while (!fila.empty())
    {
        NoTrie *atual = fila.front();
        fila.pop();
        for (auto &[c, filho] : atual->filhos)
        {
            NoTrie *f = atual->falha;
            while (f && !f->filhos[c])
                f = f->falha;
            filho->falha = f ? f->filhos[c] : raiz;
            filho->encontrados.insert(filho->encontrados.end(),
                                      filho->falha->encontrados.begin(),
                                      filho->falha->encontrados.end());
            fila.push(filho);
        }
    }
    return raiz;
}

void buscaAhoCorasick(const string &texto, const vector<string> &padroes)
{
    NoTrie *raiz = construirTrie(padroes);
    NoTrie *no = raiz;

    for (int i = 0; i < texto.length(); ++i)
    {
        char c = texto[i];
        while (no && !no->filhos[c])
            no = no->falha;
        if (!no)
            no = raiz;
        else
            no = no->filhos[c];

        for (auto &achado : no->encontrados)
            cout << "Encontrado \"" << achado << "\" na posicao " << i - achado.size() + 1 << endl;
    }
}



// ---------- ORTOGRAFIA ----------
set<string> carregarDicionario(const string &arquivo)
{
    set<string> dicionario;
    ifstream file(arquivo);
    string linha;
    while (getline(file, linha))
    {
        size_t barra = linha.find('/');
        string palavra = (barra != string::npos) ? linha.substr(0, barra) : linha;
        if (!palavra.empty())
            dicionario.insert(palavra);
    }
    return dicionario;
}

void verificarErros(const string &texto, const set<string> &dicionario)
{
    string palavra;
    for (char c : texto)
    {
        if (isalpha(c))
            palavra += tolower(c);
        else if (!palavra.empty())
        {
            if (!dicionario.count(palavra))
                cout << "Palavra desconhecida: " << palavra << endl;
            palavra.clear();
        }
    }
    if (!palavra.empty() && !dicionario.count(palavra))
        cout << "Palavra desconhecida: " << palavra << endl;
}



// ---------- MAIN ----------
int main()
{
    cout << "Digite o texto para analise (ENTER duplo para finalizar):\n";
    string linha, texto;
    while (true)
    {
        getline(cin, linha);
        if (linha.empty())
            break;
        texto += linha + " ";
    }

    set<string> dicionario = carregarDicionario("pt_BR.dic");

    while (true)
    {
        cout << "\nMENU:\n1. Busca KMP\n2. Busca com coringa\n3. Busca multipla (Aho-Corasick)\n4. Verificar palavras\n0. Sair\nEscolha: ";
        int opcao;
        cin >> opcao;
        cin.ignore();
        if (opcao == 0)
            break;

        if (opcao == 1)
        {
            string padrao;
            cout << "Digite a palavra: ";
            getline(cin, padrao);
            buscaKMP(texto, padrao);
        }
        else if (opcao == 2)
        {
            string padrao;
            cout << "Digite a palavra com '*': ";
            getline(cin, padrao);
            buscaComCoringa(texto, padrao);
        }
        else if (opcao == 3)
        {
            int n;
            cout << "Quantas palavras? ";
            cin >> n;
            cin.ignore();
            vector<string> padroes(n);
            for (int i = 0; i < n; ++i)
            {
                cout << "Palavra " << i + 1 << ": ";
                getline(cin, padroes[i]);
            }
            buscaAhoCorasick(texto, padroes);
        }
        else if (opcao == 4)
            verificarErros(texto, dicionario);
    }

    return 0;
}
