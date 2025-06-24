# 💬 Servidor de Chat Multithread em C

Um servidor de chat simples e eficiente implementado em C que suporta múltiplos clientes simultâneos usando threads POSIX e sockets TCP.

## ✨ Características

- 🚀 **Multithread**: Suporte para até 10 clientes simultâneos
- 📡 **Broadcast**: Mensagens são retransmitidas para todos os clientes conectados
- 🔄 **Thread-safe**: Uso de mutex para sincronização segura
- 🛡️ **Robusto**: Tratamento de erros e desconexões inesperadas
- 🔧 **Encerramento gracioso**: Suporte a sinais SIGINT/SIGTERM
- 🌐 **Cross-platform**: Compatível com sistemas Unix/Linux

## 🛠️ Compilação

### Pré-requisitos
- Compilador GCC
- Bibliotecas POSIX (pthread)
- Sistema Unix/Linux

### Compilar
```bash
gcc -o servidor servidor.c -lpthread
```

## 🚀 Como usar

### 1. Iniciar o servidor
```bash
./servidor
```

O servidor ficará escutando na porta `12345` e exibirá:
```
Socket criado com sucesso.
Bind realizado com sucesso.
Aguardando conexões na porta 12345...
```

### 2. Conectar clientes

#### Usando telnet:
```bash
telnet localhost 12345
```

#### Usando netcat:
```bash
nc localhost 12345
```

#### Usando cliente personalizado:
Você pode criar seu próprio cliente TCP que se conecte ao localhost na porta 12345.

### 3. Enviar mensagens
Digite qualquer mensagem e pressione Enter. A mensagem será enviada para todos os outros clientes conectados.

### 4. Encerrar o servidor
Pressione `Ctrl+C` para encerrar o servidor graciosamente.

## 📋 Exemplo de uso

**Terminal 1 (Servidor):**
```bash
$ ./servidor
Socket criado com sucesso.
Bind realizado com sucesso.
Aguardando conexões na porta 12345...
Conexão aceita de 127.0.0.1:54321
Conexão aceita de 127.0.0.1:54322
Mensagem recebida: Olá pessoal!
Mensagem recebida: Tudo bem?
```

**Terminal 2 (Cliente 1):**
```bash
$ telnet localhost 12345
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Olá pessoal!
Tudo bem?
Estou bem, obrigado!
```

**Terminal 3 (Cliente 2):**
```bash
$ telnet localhost 12345
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Olá pessoal!
Tudo bem?
Estou bem, obrigado!
Que legal!
```

## ⚙️ Configuração

### Constantes configuráveis (no código):
- `PORT`: Porta do servidor (padrão: 12345)
- `MAX_CLIENTS`: Número máximo de clientes simultâneos (padrão: 10)
- `BUFFER_SIZE`: Tamanho do buffer de mensagens (padrão: 1024 bytes)

Para alterar essas configurações, edite as defines no início do arquivo `servidor.c` e recompile.

## 🏗️ Arquitetura

### Estrutura do código:
- **Thread principal**: Aceita novas conexões
- **Thread por cliente**: Cada cliente conectado tem sua própria thread
- **Mutex**: Sincroniza acesso à lista de clientes conectados
- **Signal handlers**: Tratam encerramento gracioso

### Fluxo de funcionamento:
1. Servidor cria socket e fica escutando na porta especificada
2. Para cada nova conexão, uma thread separada é criada
3. Cada thread fica em loop recebendo mensagens do seu cliente
4. Mensagens recebidas são retransmitidas para todos os outros clientes
5. Quando um cliente desconecta, sua thread é finalizada e removida da lista

## 🐛 Tratamento de erros

O servidor trata os seguintes cenários:
- ✅ Desconexão inesperada de clientes
- ✅ Servidor lotado (mais de 10 clientes)
- ✅ Falhas na criação de threads
- ✅ Erros de rede (SIGPIPE, etc.)
- ✅ Encerramento com Ctrl+C
- ✅ Falhas de alocação de memória

## 📝 Limitações

- Máximo de 10 clientes simultâneos (configurável)
- Mensagens limitadas a 1024 bytes
- Não há autenticação ou salas separadas
- Mensagens não são persistidas
- Não há criptografia (comunicação em texto plano)

## 🤝 Contribuições

Contribuições são bem-vindas! Algumas ideias para melhorias:

- [ ] Interface gráfica para cliente
- [ ] Suporte a salas de chat
- [ ] Sistema de autenticação
- [ ] Criptografia das mensagens
- [ ] Histórico de mensagens
- [ ] Protocolo de mensagens mais robusto
- [ ] Suporte a arquivos/imagens
- [ ] Cliente web (WebSocket)

## 📄 Licença

Este projeto está sob a licença MIT. Veja o arquivo `LICENSE` para mais detalhes.

## 🔗 Links úteis

- [Documentação Socket Programming](https://beej.us/guide/bgnet/)
- [POSIX Threads Tutorial](https://computing.llnl.gov/tutorials/pthreads/)
- [TCP/IP Sockets in C](https://cs.baylor.edu/~donahoo/practical/CSockets/)

---

⭐ Se este projeto foi útil para você, considere dar uma estrela!

📧 Para dúvidas ou sugestões, abra uma issue.
