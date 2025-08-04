#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <map>
#include <chrono>
#include <random>
#include <unordered_map>
#include <iomanip>
#include <limits>
#include <fstream>
#include <sstream>
#include <unordered_set>

// Funcao para construir o vetor de prefixo (lps) do KMP
std::vector<int> buildLPS(const std::string &pattern)
{
    int m = pattern.size();
    std::vector<int> lps(m, 0);
    int len = 0;
    int i = 1;
    while (i < m)
    {
        if (pattern[i] == pattern[len])
        {
            len++;
            lps[i] = len;
            i++;
        }
        else
        {
            if (len != 0)
            {
                len = lps[len - 1];
            }
            else
            {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}

// Funcao para verificar se um caractere corresponde ao padrao (incluindo wildcard)
bool charMatch(char textChar, char patternChar)
{
    return patternChar == '*' || patternChar == textChar;
}

// Funcao para encontrar todas as ocorrencias com suporte a wildcard
std::vector<int> findPatternWithWildcard(const std::string &text, const std::string &pattern)
{
    std::vector<int> positions;
    int n = text.size();
    int m = pattern.size();

    if (m == 0)
        return positions;

    // Para cada posição possível no texto
    for (int i = 0; i <= n - m; i++)
    {
        bool match = true;
        // Verificar se o padrão casa a partir da posição i
        for (int j = 0; j < m; j++)
        {
            if (!charMatch(text[i + j], pattern[j]))
            {
                match = false;
                break;
            }
        }
        if (match)
        {
            positions.push_back(i);
        }
    }

    return positions;
}

// Funcao KMP para encontrar todas as ocorrencias
std::vector<int> KMP(const std::string &text, const std::string &pattern)
{
    std::vector<int> positions;
    int n = text.size();
    int m = pattern.size();
    if (m == 0)
        return positions;
    std::vector<int> lps = buildLPS(pattern);
    int i = 0, j = 0;
    while (i < n)
    {
        if (pattern[j] == text[i])
        {
            i++;
            j++;
        }
        if (j == m)
        {
            positions.push_back(i - j);
            j = lps[j - 1];
        }
        else if (i < n && pattern[j] != text[i])
        {
            if (j != 0)
            {
                j = lps[j - 1];
            }
            else
            {
                i++;
            }
        }
    }
    return positions;
}

// Classe para criptografia por permutação
class CifraPermutacao
{
private:
    std::string alfabeto = "abcdefghijklmnopqrstuvwxyz";
    std::map<char, char> chaveCriptografia;
    std::map<char, char> chaveDescriptografia;

public:
    // Gerar chave aleatória
    void gerarChaveAleatoria()
    {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::string alfabetoShuffled = alfabeto;
        std::shuffle(alfabetoShuffled.begin(), alfabetoShuffled.end(), gen);

        chaveCriptografia.clear();
        chaveDescriptografia.clear();

        for (size_t i = 0; i < alfabeto.length(); i++)
        {
            chaveCriptografia[alfabeto[i]] = alfabetoShuffled[i];
            chaveDescriptografia[alfabetoShuffled[i]] = alfabeto[i];
        }
    }

    // Definir chave manualmente
    void definirChave(const std::string &novaChave)
    {
        if (novaChave.length() != alfabeto.length())
        {
            throw std::invalid_argument("A chave deve ter 26 caracteres");
        }

        chaveCriptografia.clear();
        chaveDescriptografia.clear();

        for (size_t i = 0; i < alfabeto.length(); i++)
        {
            chaveCriptografia[alfabeto[i]] = novaChave[i];
            chaveDescriptografia[novaChave[i]] = alfabeto[i];
        }
    }

    // Criptografar texto
    std::string criptografar(const std::string &texto)
    {
        std::string resultado = texto;
        for (char &c : resultado)
        {
            if (isalpha(c))
            {
                char original = tolower(c);
                if (chaveCriptografia.find(original) != chaveCriptografia.end())
                {
                    c = isupper(c) ? toupper(chaveCriptografia[original]) : chaveCriptografia[original];
                }
            }
        }
        return resultado;
    }

    // Descriptografar texto
    std::string descriptografar(const std::string &texto)
    {
        std::string resultado = texto;
        for (char &c : resultado)
        {
            if (isalpha(c))
            {
                char original = tolower(c);
                if (chaveDescriptografia.find(original) != chaveDescriptografia.end())
                {
                    c = isupper(c) ? toupper(chaveDescriptografia[original]) : chaveDescriptografia[original];
                }
            }
        }
        return resultado;
    }

    // Obter chave atual
    std::string obterChave()
    {
        std::string chave;
        for (char c : alfabeto)
        {
            chave += chaveCriptografia[c];
        }
        return chave;
    }

    // Mostrar mapeamento da chave
    void mostrarMapeamento()
    {
        std::cout << "Mapeamento da chave:" << std::endl;
        std::cout << "Original:  " << alfabeto << std::endl;
        std::cout << "Cifrada:   " << obterChave() << std::endl;
    }
};

// Classe para quebra de cifra usando análise de frequência
class QuebraCifra
{
private:
    std::string alfabeto = "abcdefghijklmnopqrstuvwxyz";

    // Frequências típicas das letras em português
    std::map<char, double> frequenciasPortugues = {
        {'a', 14.63}, {'b', 1.04}, {'c', 3.88}, {'d', 4.99}, {'e', 12.57}, {'f', 1.02}, {'g', 1.30}, {'h', 1.28}, {'i', 6.18}, {'j', 0.40}, {'k', 0.02}, {'l', 2.78}, {'m', 4.74}, {'n', 5.05}, {'o', 10.73}, {'p', 2.52}, {'q', 1.20}, {'r', 6.53}, {'s', 7.81}, {'t', 4.34}, {'u', 4.63}, {'v', 1.67}, {'w', 0.01}, {'x', 0.21}, {'y', 0.01}, {'z', 0.47}};

    // Calcular frequência de letras no texto
    std::map<char, double> calcularFrequencias(const std::string &texto)
    {
        std::map<char, int> contagem;
        int totalLetras = 0;

        for (char c : texto)
        {
            if (isalpha(c))
            {
                contagem[tolower(c)]++;
                totalLetras++;
            }
        }

        std::map<char, double> frequencias;
        for (const auto &pair : contagem)
        {
            frequencias[pair.first] = (double)pair.second / totalLetras * 100;
        }

        return frequencias;
    }

    // Calcular score de similaridade com frequências do português
    double calcularScore(const std::map<char, double> &freqTexto)
    {
        double score = 0;
        for (const auto &pair : frequenciasPortugues)
        {
            char letra = pair.first;
            double freqEsperada = pair.second;
            double freqAtual = (freqTexto.find(letra) != freqTexto.end()) ? freqTexto.at(letra) : 0;
            score += -abs(freqEsperada - freqAtual);
        }
        return score;
    }

public:
    // Quebrar cifra usando análise de frequência
    std::string quebrarCifra(const std::string &textoCifrado)
    {
        std::cout << "Analisando frequencias das letras..." << std::endl;

        auto freqCifrado = calcularFrequencias(textoCifrado);

        std::cout << "Frequencias no texto cifrado:" << std::endl;
        for (const auto &pair : freqCifrado)
        {
            std::cout << pair.first << ": " << std::fixed << std::setprecision(2) << pair.second << "% ";
        }
        std::cout << std::endl;

        // Ordenar letras por frequência (cifrado vs português)
        std::vector<std::pair<char, double>> freqCifradoVec(freqCifrado.begin(), freqCifrado.end());
        std::vector<std::pair<char, double>> freqPortuguesVec(frequenciasPortugues.begin(), frequenciasPortugues.end());

        std::sort(freqCifradoVec.begin(), freqCifradoVec.end(),
                  [](const auto &a, const auto &b)
                  { return a.second > b.second; });
        std::sort(freqPortuguesVec.begin(), freqPortuguesVec.end(),
                  [](const auto &a, const auto &b)
                  { return a.second > b.second; });

        // Criar mapeamento baseado na frequência
        std::map<char, char> mapeamento;
        for (size_t i = 0; i < freqCifradoVec.size() && i < freqPortuguesVec.size(); i++)
        {
            mapeamento[freqCifradoVec[i].first] = freqPortuguesVec[i].first;
        }

        // Aplicar mapeamento
        std::string resultado = textoCifrado;
        for (char &c : resultado)
        {
            if (isalpha(c))
            {
                char original = tolower(c);
                if (mapeamento.find(original) != mapeamento.end())
                {
                    c = isupper(c) ? toupper(mapeamento[original]) : mapeamento[original];
                }
            }
        }

        std::cout << "\nMapeamento sugerido (frequencia):" << std::endl;
        for (const auto &pair : mapeamento)
        {
            std::cout << pair.first << " -> " << pair.second << " ";
        }
        std::cout << std::endl;

        return resultado;
    }

    // Quebrar cifra usando força bruta (para textos pequenos)
    std::string quebrarCifraForcaBruta(const std::string &textoCifrado)
    {
        std::cout << "Tentando quebra por forca bruta..." << std::endl;

        std::string melhorResultado = textoCifrado;
        double melhorScore = -std::numeric_limits<double>::max();

        // Gerar permutações aleatórias e testar
        std::random_device rd;
        std::mt19937 gen(rd());

        for (int tentativa = 0; tentativa < 1000; tentativa++)
        {
            std::string alfabetoShuffled = alfabeto;
            std::shuffle(alfabetoShuffled.begin(), alfabetoShuffled.end(), gen);

            // Aplicar permutação
            std::string resultado = textoCifrado;
            for (char &c : resultado)
            {
                if (isalpha(c))
                {
                    char original = tolower(c);
                    size_t pos = alfabeto.find(original);
                    if (pos != std::string::npos)
                    {
                        c = isupper(c) ? toupper(alfabetoShuffled[pos]) : alfabetoShuffled[pos];
                    }
                }
            }

            // Calcular score
            auto freq = calcularFrequencias(resultado);
            double score = calcularScore(freq);

            if (score > melhorScore)
            {
                melhorScore = score;
                melhorResultado = resultado;
            }
        }

        std::cout << "Melhor score encontrado: " << melhorScore << std::endl;
        return melhorResultado;
    }
};

// Classe para verificação ortográfica
class VerificadorOrtografico
{
private:
    std::unordered_set<std::string> dicionario;
    std::string arquivoDicionario = "pt_BR.dic";

    // Limpar palavra (remover pontuação e converter para minúsculo)
    std::string limparPalavra(const std::string &palavra)
    {
        std::string limpa;
        for (char c : palavra)
        {
            if (isalpha(c))
            {
                limpa += tolower(c);
            }
        }
        return limpa;
    }

    // Extrair palavras de um texto
    std::vector<std::string> extrairPalavras(const std::string &texto)
    {
        std::vector<std::string> palavras;
        std::stringstream ss(texto);
        std::string palavra;

        while (ss >> palavra)
        {
            std::string limpa = limparPalavra(palavra);
            if (!limpa.empty())
            {
                palavras.push_back(limpa);
            }
        }

        return palavras;
    }

public:
    // Carregar dicionário do arquivo
    bool carregarDicionario()
    {
        std::ifstream arquivo(arquivoDicionario);
        if (!arquivo.is_open())
        {
            std::cout << "Erro: Nao foi possivel abrir o arquivo " << arquivoDicionario << std::endl;
            return false;
        }

        std::string linha;
        int contador = 0;
        while (std::getline(arquivo, linha))
        {
            // Remover espaços em branco e caracteres especiais
            std::string palavra = limparPalavra(linha);
            if (!palavra.empty())
            {
                dicionario.insert(palavra);
                contador++;
            }
        }

        arquivo.close();
        std::cout << "Dicionario carregado com " << contador << " palavras." << std::endl;
        return true;
    }

    // Verificar se uma palavra está no dicionário
    bool verificarPalavra(const std::string &palavra)
    {
        std::string limpa = limparPalavra(palavra);
        return dicionario.find(limpa) != dicionario.end();
    }

    // Encontrar erros ortográficos em um texto
    std::vector<std::pair<std::string, int>> encontrarErros(const std::string &texto)
    {
        std::vector<std::pair<std::string, int>> erros;
        std::vector<std::string> palavras = extrairPalavras(texto);

        for (size_t i = 0; i < palavras.size(); i++)
        {
            if (!verificarPalavra(palavras[i]))
            {
                erros.push_back({palavras[i], i + 1}); // i+1 para posição baseada em 1
            }
        }

        return erros;
    }

    // Encontrar erros ortográficos apenas para palavras que começam com uma letra específica
    std::vector<std::pair<std::string, int>> encontrarErrosPorLetra(const std::string &texto, char letra)
    {
        std::vector<std::pair<std::string, int>> erros;
        std::vector<std::string> palavras = extrairPalavras(texto);

        for (size_t i = 0; i < palavras.size(); i++)
        {
            if (!palavras[i].empty() && palavras[i][0] == tolower(letra))
            {
                if (!verificarPalavra(palavras[i]))
                {
                    erros.push_back({palavras[i], i + 1});
                }
            }
        }

        return erros;
    }

    // Mostrar estatísticas do dicionário
    void mostrarEstatisticas()
    {
        std::cout << "Estatisticas do dicionario:" << std::endl;
        std::cout << "Total de palavras: " << dicionario.size() << std::endl;

        // Contar palavras por letra inicial
        std::map<char, int> contagemPorLetra;
        for (const auto &palavra : dicionario)
        {
            if (!palavra.empty())
            {
                contagemPorLetra[palavra[0]]++;
            }
        }

        std::cout << "Palavras por letra inicial:" << std::endl;
        for (char c = 'a'; c <= 'z'; c++)
        {
            if (contagemPorLetra.find(c) != contagemPorLetra.end())
            {
                std::cout << c << ": " << contagemPorLetra[c] << " ";
            }
        }
        std::cout << std::endl;
    }

    // Sugerir correções para uma palavra
    std::vector<std::string> sugerirCorrecoes(const std::string &palavra)
    {
        std::vector<std::string> sugestoes;
        std::string palavraLimpa = limparPalavra(palavra);

        // Buscar palavras similares no dicionário
        for (const auto &palavraDict : dicionario)
        {
            if (palavraDict.length() >= palavraLimpa.length() - 1 &&
                palavraDict.length() <= palavraLimpa.length() + 1)
            {

                // Calcular similaridade simples (pode ser melhorada com algoritmo de Levenshtein)
                int diferencas = 0;
                size_t minLen = std::min(palavraDict.length(), palavraLimpa.length());

                for (size_t i = 0; i < minLen; i++)
                {
                    if (palavraDict[i] != palavraLimpa[i])
                    {
                        diferencas++;
                    }
                }
                diferencas += abs((int)palavraDict.length() - (int)palavraLimpa.length());

                if (diferencas <= 2)
                { // Máximo 2 diferenças
                    sugestoes.push_back(palavraDict);
                }
            }
        }

        // Limitar a 5 sugestões
        if (sugestoes.size() > 5)
        {
            sugestoes.resize(5);
        }

        return sugestoes;
    }
};

// Estrutura para o nó da árvore de sufixos do Aho-Corasick
struct TrieNode
{
    std::map<char, TrieNode *> children;
    TrieNode *failure;
    std::vector<int> output;
    bool isEndOfWord;

    TrieNode() : failure(nullptr), isEndOfWord(false) {}
};

// Classe para implementar o algoritmo Aho-Corasick
class AhoCorasick
{
private:
    TrieNode *root;
    std::vector<std::string> patterns;

    void buildTrie()
    {
        root = new TrieNode();

        for (int i = 0; i < patterns.size(); i++)
        {
            TrieNode *current = root;
            for (char c : patterns[i])
            {
                if (current->children.find(c) == current->children.end())
                {
                    current->children[c] = new TrieNode();
                }
                current = current->children[c];
            }
            current->isEndOfWord = true;
            current->output.push_back(i);
        }
    }

    void buildFailureLinks()
    {
        std::queue<TrieNode *> q;

        // Inicializar links de falha para nós de profundidade 1
        for (auto &pair : root->children)
        {
            pair.second->failure = root;
            q.push(pair.second);
        }

        while (!q.empty())
        {
            TrieNode *current = q.front();
            q.pop();

            for (auto &pair : current->children)
            {
                char c = pair.first;
                TrieNode *child = pair.second;
                q.push(child);

                TrieNode *failure = current->failure;
                while (failure && failure->children.find(c) == failure->children.end())
                {
                    failure = failure->failure;
                }

                child->failure = failure ? failure->children[c] : root;
                child->output.insert(child->output.end(),
                                     child->failure->output.begin(),
                                     child->failure->output.end());
            }
        }
    }

public:
    AhoCorasick(const std::vector<std::string> &patterns) : patterns(patterns)
    {
        buildTrie();
        buildFailureLinks();
    }

    std::vector<std::pair<int, int>> search(const std::string &text)
    {
        std::vector<std::pair<int, int>> results; // (posição, índice do padrão)
        TrieNode *current = root;

        for (int i = 0; i < text.length(); i++)
        {
            char c = text[i];

            while (current && current->children.find(c) == current->children.end())
            {
                current = current->failure;
            }

            if (!current)
            {
                current = root;
                continue;
            }

            current = current->children[c];

            for (int patternIndex : current->output)
            {
                int startPos = i - patterns[patternIndex].length() + 1;
                results.push_back({startPos, patternIndex});
            }
        }

        return results;
    }

    ~AhoCorasick()
    {
        // Implementar limpeza da árvore se necessário
    }
};

// Função para busca ingênua (força bruta)
std::vector<int> buscaIngenua(const std::string &text, const std::string &pattern)
{
    std::vector<int> positions;
    int n = text.size();
    int m = pattern.size();

    for (int i = 0; i <= n - m; i++)
    {
        bool match = true;
        for (int j = 0; j < m; j++)
        {
            if (text[i + j] != pattern[j])
            {
                match = false;
                break;
            }
        }
        if (match)
        {
            positions.push_back(i);
        }
    }

    return positions;
}

int main()
{
    std::string texto;
    std::cout << "Digite o texto: ";
    std::getline(std::cin, texto);

    std::cout << "\n=== SISTEMA DE BUSCA E CRIPTOGRAFIA AVANCADA ===" << std::endl;
    std::cout << "1. Busca simples (KMP com wildcard)" << std::endl;
    std::cout << "2. Busca multipla (Aho-Corasick)" << std::endl;
    std::cout << "3. Analise de desempenho comparativo" << std::endl;
    std::cout << "4. Criptografia por permutacao" << std::endl;
    std::cout << "5. Quebra de cifra" << std::endl;
    std::cout << "6. Verificacao ortografica" << std::endl;
    std::cout << "7. Sair" << std::endl;

    while (true)
    {
        std::cout << "\nEscolha uma opcao (1-7): ";
        std::string opcao;
        std::getline(std::cin, opcao);

        if (opcao == "1")
        {
            // Busca simples
            std::cout << "\n=== BUSCA SIMPLES ===" << std::endl;
            std::cout << "Dica: Use * como coringa para representar qualquer caractere." << std::endl;

            while (true)
            {
                std::cout << "Digite a palavra a ser buscada (ou '-1' para voltar): ";
                std::string palavra;
                std::getline(std::cin, palavra);

                if (palavra == "-1")
                    break;

                auto start = std::chrono::high_resolution_clock::now();
                std::vector<int> ocorrencias = findPatternWithWildcard(texto, palavra);
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

                if (ocorrencias.empty())
                {
                    std::cout << "Nenhuma ocorrencia encontrada." << std::endl;
                }
                else
                {
                    std::cout << "Ocorrencias encontradas nas posicoes: ";
                    for (int pos : ocorrencias)
                    {
                        std::cout << (pos + 1) << " ";
                    }
                    std::cout << std::endl;
                }
                std::cout << "Tempo de busca: " << duration.count() << " microssegundos" << std::endl;
            }
        }
        else if (opcao == "2")
        {
            // Busca múltipla
            std::cout << "\n=== BUSCA MÚLTIPLA (AHO-CORASICK) ===" << std::endl;
            std::vector<std::string> palavras;

            std::cout << "Digite as palavras a serem buscadas (uma por linha, digite '-1' para finalizar):" << std::endl;
            while (true)
            {
                std::string palavra;
                std::getline(std::cin, palavra);
                if (palavra == "-1")
                    break;
                palavras.push_back(palavra);
            }

            if (!palavras.empty())
            {
                auto start = std::chrono::high_resolution_clock::now();
                AhoCorasick ac(palavras);
                auto results = ac.search(texto);
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

                std::cout << "\nResultados da busca múltipla:" << std::endl;
                for (auto &result : results)
                {
                    std::cout << "Palavra '" << palavras[result.second] << "' encontrada na posicao " << (result.first + 1) << std::endl;
                }
                std::cout << "Tempo total de busca: " << duration.count() << " microssegundos" << std::endl;
            }
        }
        else if (opcao == "3")
        {
            // Análise de desempenho
            std::cout << "\n=== ANÁLISE DE DESEMPENHO COMPARATIVO ===" << std::endl;
            std::cout << "Digite uma palavra para testar: ";
            std::string palavra;
            std::getline(std::cin, palavra);

            // Teste busca ingênua
            auto start1 = std::chrono::high_resolution_clock::now();
            std::vector<int> result1 = buscaIngenua(texto, palavra);
            auto end1 = std::chrono::high_resolution_clock::now();
            auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);

            // Teste KMP
            auto start2 = std::chrono::high_resolution_clock::now();
            std::vector<int> result2 = KMP(texto, palavra);
            auto end2 = std::chrono::high_resolution_clock::now();
            auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);

            // Teste Aho-Corasick (uma palavra)
            auto start3 = std::chrono::high_resolution_clock::now();
            AhoCorasick ac({palavra});
            auto result3 = ac.search(texto);
            auto end3 = std::chrono::high_resolution_clock::now();
            auto duration3 = std::chrono::duration_cast<std::chrono::microseconds>(end3 - start3);

            std::cout << "\nResultados da analise:" << std::endl;
            std::cout << "Busca Ingênua: " << duration1.count() << " microssegundos" << std::endl;
            std::cout << "KMP: " << duration2.count() << " microssegundos" << std::endl;
            std::cout << "Aho-Corasick: " << duration3.count() << " microssegundos" << std::endl;

            std::cout << "\nNumero de ocorrencias encontradas:" << std::endl;
            std::cout << "Busca Ingenua: " << result1.size() << std::endl;
            std::cout << "KMP: " << result2.size() << std::endl;
            std::cout << "Aho-Corasick: " << result3.size() << std::endl;
        }
        else if (opcao == "4")
        {
            // Criptografia por permutação
            std::cout << "\n=== CRIPTOGRAFIA POR PERMUTACAO ===" << std::endl;
            CifraPermutacao cifra;

            std::cout << "1. Gerar chave aleatoria" << std::endl;
            std::cout << "2. Definir chave manualmente" << std::endl;
            std::cout << "3. Voltar" << std::endl;

            std::string subOpcao;
            std::getline(std::cin, subOpcao);

            if (subOpcao == "1")
            {
                cifra.gerarChaveAleatoria();
                cifra.mostrarMapeamento();

                std::cout << "\nDigite o texto a ser criptografado: ";
                std::string textoParaCriptografar;
                std::getline(std::cin, textoParaCriptografar);

                std::string textoCifrado = cifra.criptografar(textoParaCriptografar);
                std::cout << "Texto criptografado: " << textoCifrado << std::endl;

                std::string textoDescriptografado = cifra.descriptografar(textoCifrado);
                std::cout << "Texto descriptografado: " << textoDescriptografado << std::endl;
            }
            else if (subOpcao == "2")
            {
                std::cout << "Digite a chave (26 caracteres, permutacao do alfabeto): ";
                std::string chave;
                std::getline(std::cin, chave);

                try
                {
                    cifra.definirChave(chave);
                    cifra.mostrarMapeamento();

                    std::cout << "\nDigite o texto a ser criptografado: ";
                    std::string textoParaCriptografar;
                    std::getline(std::cin, textoParaCriptografar);

                    std::string textoCifrado = cifra.criptografar(textoParaCriptografar);
                    std::cout << "Texto criptografado: " << textoCifrado << std::endl;

                    std::string textoDescriptografado = cifra.descriptografar(textoCifrado);
                    std::cout << "Texto descriptografado: " << textoDescriptografado << std::endl;
                }
                catch (const std::exception &e)
                {
                    std::cout << "Erro: " << e.what() << std::endl;
                }
            }
        }
        else if (opcao == "5")
        {
            // Quebra de cifra
            std::cout << "\n=== QUEBRA DE CIFRA ===" << std::endl;
            std::cout << "Digite o texto cifrado para tentar quebrar: ";
            std::string textoCifrado;
            std::getline(std::cin, textoCifrado);

            QuebraCifra quebrador;

            std::cout << "1. Analise de frequencia" << std::endl;
            std::cout << "2. Forca bruta (limitado)" << std::endl;

            std::string subOpcao;
            std::getline(std::cin, subOpcao);

            if (subOpcao == "1")
            {
                std::string resultado = quebrador.quebrarCifra(textoCifrado);
                std::cout << "\nTexto decifrado (analise de frequencia):" << std::endl;
                std::cout << resultado << std::endl;
            }
            else if (subOpcao == "2")
            {
                std::string resultado = quebrador.quebrarCifraForcaBruta(textoCifrado);
                std::cout << "\nTexto decifrado (forca bruta):" << std::endl;
                std::cout << resultado << std::endl;
            }
        }
        else if (opcao == "6")
        {
            // Verificação ortográfica
            std::cout << "\n=== VERIFICACAO ORTOGRAFICA ===" << std::endl;

            VerificadorOrtografico verificador;

            // Tentar carregar o dicionário
            if (!verificador.carregarDicionario())
            {
                std::cout << "Nao foi possivel carregar o dicionario. Verificacao ortografica nao disponivel." << std::endl;
                continue;
            }

            std::cout << "\n1. Verificar erros em todo o texto" << std::endl;
            std::cout << "2. Verificar erros apenas para palavras que comecam com uma letra especifica" << std::endl;
            std::cout << "3. Verificar uma palavra especifica" << std::endl;
            std::cout << "4. Mostrar estatisticas do dicionario" << std::endl;
            std::cout << "5. Voltar" << std::endl;

            std::string subOpcao;
            std::getline(std::cin, subOpcao);

            if (subOpcao == "1")
            {
                auto start = std::chrono::high_resolution_clock::now();
                auto erros = verificador.encontrarErros(texto);
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

                if (erros.empty())
                {
                    std::cout << "Nenhum erro ortografico encontrado!" << std::endl;
                }
                else
                {
                    std::cout << "Erros ortograficos encontrados (" << erros.size() << "):" << std::endl;
                    for (const auto &erro : erros)
                    {
                        std::cout << "Palavra " << erro.second << ": '" << erro.first << "'";

                        // Sugerir correções
                        auto sugestoes = verificador.sugerirCorrecoes(erro.first);
                        if (!sugestoes.empty())
                        {
                            std::cout << " - Sugestoes: ";
                            for (size_t i = 0; i < sugestoes.size(); i++)
                            {
                                std::cout << sugestoes[i];
                                if (i < sugestoes.size() - 1)
                                    std::cout << ", ";
                            }
                        }
                        std::cout << std::endl;
                    }
                }
                std::cout << "Tempo de verificacao: " << duration.count() << " microssegundos" << std::endl;
            }
            else if (subOpcao == "2")
            {
                std::cout << "Digite a letra para filtrar (ex: a, q): ";
                std::string letraStr;
                std::getline(std::cin, letraStr);

                if (letraStr.length() == 1 && isalpha(letraStr[0]))
                {
                    char letra = tolower(letraStr[0]);

                    auto start = std::chrono::high_resolution_clock::now();
                    auto erros = verificador.encontrarErrosPorLetra(texto, letra);
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

                    if (erros.empty())
                    {
                        std::cout << "Nenhum erro ortografico encontrado para palavras que comecam com '" << letra << "'!" << std::endl;
                    }
                    else
                    {
                        std::cout << "Erros ortograficos para palavras que comecam com '" << letra << "' (" << erros.size() << "):" << std::endl;
                        for (const auto &erro : erros)
                        {
                            std::cout << "Palavra " << erro.second << ": '" << erro.first << "'" << std::endl;
                        }
                    }
                    std::cout << "Tempo de verificacao: " << duration.count() << " microssegundos" << std::endl;
                }
                else
                {
                    std::cout << "Letra invalida!" << std::endl;
                }
            }
            else if (subOpcao == "3")
            {
                std::cout << "Digite a palavra para verificar: ";
                std::string palavra;
                std::getline(std::cin, palavra);

                if (verificador.verificarPalavra(palavra))
                {
                    std::cout << "A palavra '" << palavra << "' esta correta!" << std::endl;
                }
                else
                {
                    std::cout << "A palavra '" << palavra << "' nao foi encontrada no dicionario." << std::endl;

                    auto sugestoes = verificador.sugerirCorrecoes(palavra);
                    if (!sugestoes.empty())
                    {
                        std::cout << "Sugestoes de correcao: ";
                        for (size_t i = 0; i < sugestoes.size(); i++)
                        {
                            std::cout << sugestoes[i];
                            if (i < sugestoes.size() - 1)
                                std::cout << ", ";
                        }
                        std::cout << std::endl;
                    }
                }
            }
            else if (subOpcao == "4")
            {
                verificador.mostrarEstatisticas();
            }
        }
        else if (opcao == "7")
        {
            std::cout << "Programa encerrado." << std::endl;
            break;
        }
        else
        {
            std::cout << "Opcao invalida. Tente novamente." << std::endl;
        }
    }
    return 0;
}