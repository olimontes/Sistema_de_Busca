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

// ---------- CIFRA ----------
string aplicarCifra(const string &texto, const string &chave, bool decifrar = false)
{
    string alfabeto = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    unordered_map<char, char> mapa;

    if (!decifrar)
        for (int i = 0; i < 26; ++i)
            mapa[alfabeto[i]] = chave[i];
    else
        for (int i = 0; i < 26; ++i)
            mapa[chave[i]] = alfabeto[i];

    string resultado;
    for (char c : texto)
    {
        char maiusculo = toupper(c);
        resultado += isalpha(maiusculo) ? mapa[maiusculo] : c;
    }
    return resultado;
}

string quebrarCifra(const string &criptografado)
{
    string frequencia = "ETAOINSHRDLCUMWFGYPBVKJXQZ";
    unordered_map<char, int> contagem;
    for (char c : criptografado)
        if (isalpha(c))
            contagem[toupper(c)]++;

    vector<pair<int, char>> ordenado;
    for (auto &[c, f] : contagem)
        ordenado.emplace_back(f, c);
    sort(ordenado.rbegin(), ordenado.rend());

    unordered_map<char, char> mapa;
    for (int i = 0; i < ordenado.size() && i < frequencia.size(); ++i)
        mapa[ordenado[i].second] = frequencia[i];

    string tentativa;
    for (char c : criptografado)
    {
        char maiusculo = toupper(c);
        tentativa += isalpha(maiusculo) ? mapa[maiusculo] : c;
    }
    return tentativa;
}

// ---------- DISTANCIA DE EDICAO ----------
int distanciaLevenshtein(const string &a, const string &b)
{
    int m = a.size(), n = b.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));
    for (int i = 0; i <= m; ++i)
        dp[i][0] = i;
    for (int j = 0; j <= n; ++j)
        dp[0][j] = j;

    for (int i = 1; i <= m; ++i)
        for (int j = 1; j <= n; ++j)
            dp[i][j] = min({dp[i - 1][j] + 1,
                            dp[i][j - 1] + 1,
                            dp[i - 1][j - 1] + (a[i - 1] != b[j - 1])});
    return dp[m][n];
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

void sugerirCorrecao(const string &palavra, const set<string> &dicionario)
{
    vector<pair<int, string>> sugestoes;
    for (const string &w : dicionario)
        sugestoes.emplace_back(distanciaLevenshtein(palavra, w), w);
    sort(sugestoes.begin(), sugestoes.end());
    cout << "Sugestoes para \"" << palavra << "\":" << endl;
    for (int i = 0; i < min(5, (int)sugestoes.size()); ++i)
        cout << "- " << sugestoes[i].second << " (distancia " << sugestoes[i].first << ")" << endl;
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
        cout << "\nMENU:\n1. Busca KMP\n2. Busca com coringa\n3. Busca multipla (Aho-Corasick)\n4. Cifrar/quebrar cifra\n5. Verificar palavras\n6. Sugerir correcao\n0. Sair\nEscolha: ";
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
        {
            string chave = "QWERTYUIOPASDFGHJKLZXCVBNM";
            string cifrado = aplicarCifra(texto, chave);
            cout << "Texto cifrado: " << cifrado << endl;
            cout << "Texto decifrado com chave: " << aplicarCifra(cifrado, chave, true) << endl;
            cout << "Tentando quebrar: " << quebrarCifra(cifrado) << endl;
        }
        else if (opcao == 5)
            verificarErros(texto, dicionario);
        else if (opcao == 6)
        {
            string palavra;
            cout << "Digite a palavra: ";
            getline(cin, palavra);
            sugerirCorrecao(palavra, dicionario);
        }
    }

    return 0;
}
