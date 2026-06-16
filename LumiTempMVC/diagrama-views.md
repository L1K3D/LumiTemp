```mermaid
classDiagram
    %% ==========================================
    %% ARQUIVOS DE CONFIGURAÇÃO GLOBAL DAS VIEWS
    %% ==========================================
    class Globais {
        +_ViewStart.cshtml (Define o Layout padrão)
        +_ViewImports.cshtml (Importa Namespaces/TagHelpers)
    }

    %% ==========================================
    %% PASTAS E ARQUIVOS .CSHTML (Telas por Módulo)
    %% ==========================================
    package "Dashboard" {
        class Dashboard_Views {
            +Index.cshtml (Painel principal)
        }
    }

    package "EmpresaParceira" {
        class EmpresaParceira_Views {
            +Form.cshtml (Cadastro/Edição)
            +Index.cshtml (Listagem de Empresas)
        }
    }

    package "FiwareData" {
        class FiwareData_Views {
            +Index.cshtml (Telemetria do Sensor)
        }
    }

    package "Funcionario" {
        class Funcionario_Views {
            +Index.cshtml (Listagem de Funcionários)
            +Form.cshtml (Cadastro/Edição)
            +ConsultaAvancada.cshtml (Filtros de pesquisa)
            +pvGridFuncionarios.cshtml (Partial View - Tabela assíncrona)
        }
    }

    package "Home" {
        class Home_Views {
            +Index.cshtml (Página de boas-vindas)
            +Privacy.cshtml (Termos de privacidade)
        }
    }

    package "Login" {
        class Login_Views {
            +Index.cshtml (Tela de autenticação)
        }
    }

    package "Sensor" {
        class Sensor_Views {
            +Index.cshtml (Listagem de Sensores)
            +Form.cshtml (Cadastro/Edição)
            +ConsultaAvancada.cshtml (Filtros de pesquisa)
            +pvGridSensores.cshtml (Partial View - Tabela assíncrona)
        }
    }

    package "Sobre" {
        class Sobre_Views {
            +Index.cshtml (Informações do sistema)
        }
    }

    %% ==========================================
    %% PASTA SHARED (Componentes Compartilhados)
    %% ==========================================
    package "Shared (Compartilhado)" {
        class Shared_Views {
            +_Layout.cshtml (Template estrutural: Topo/Menu/Rodapé)
            +_ValidationScriptsPartial.cshtml (Scripts de validação front-end)
            +Error.cshtml (Tela global de exceções)
        }
    }

    %% ==========================================
    %% RELACIONAMENTOS E DEPENDÊNCIAS DAS VIEWS
    %% ==========================================
    
    %% O Layout envelopa as páginas principais
    Shared_Views --> Dashboard_Views : Estrutura base via _Layout
    Shared_Views --> EmpresaParceira_Views : Estrutura base via _Layout
    Shared_Views --> FiwareData_Views : Estrutura base via _Layout
    Shared_Views --> Funcionario_Views : Estrutura base via _Layout
    Shared_Views --> Home_Views : Estrutura base via _Layout
    Shared_Views --> Sensor_Views : Estrutura base via _Layout
    Shared_Views --> Sobre_Views : Estrutura base via _Layout

    %% Scripts de validação são injetados nos formulários
    Shared_Views ..> EmpresaParceira_Views : Fornece _ValidationScripts para o Form
    Shared_Views ..> Funcionario_Views : Fornece _ValidationScripts para o Form
    Shared_Views ..> Sensor_Views : Fornece _ValidationScripts para o Form
```
