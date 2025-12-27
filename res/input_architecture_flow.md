# Architecture des Inputs - Blood Bowl

## Vue d'Ensemble du Flux

```
┌──────────────────────────────────────────────────────────────────┐
│                        MAIN GAME LOOP                             │
│  while (window.isOpen()) {                                        │
│      while (window.pollEvent(event)) { ... }                      │
│  }                                                                 │
└────────────────────────────┬─────────────────────────────────────┘
                             │
                             ▼
┌──────────────────────────────────────────────────────────────────┐
│                      SCENE MANAGER                                │
│  void handleEvent(const sf::Event& event, ...) {                  │
│      Screen* current = getCurrent();                              │
│      current->handleEvent(event, window);                         │
│  }                                                                 │
└────────────────────────────┬─────────────────────────────────────┘
                             │
              ┌──────────────┴──────────────┐
              │                             │
              ▼                             ▼
┌─────────────────────────┐   ┌─────────────────────────┐
│   HomeScreen            │   │   GameScreen            │
│   TeamCreationScreen    │   │   MatchCreationScreen   │
│   EndGameScreen         │   │                         │
└─────────────────────────┘   └─────────┬───────────────┘
              │                         │
              │                         │
              ▼                         ▼
┌─────────────────────────┐   ┌─────────────────────────┐
│  UI-Only Handling       │   │  Mixed Handling         │
│  • Button clicks        │   │  1. UI layer (dice)     │
│  • Navigation           │   │  2. → InputHandler      │
│  • Menu interactions    │   │  3. Global shortcuts    │
└─────────────────────────┘   └─────────┬───────────────┘
                                        │
                                        ▼
                              ┌─────────────────────────┐
                              │   INPUT HANDLER         │
                              │  (Gameplay Logic)       │
                              │  • Character selection  │
                              │  • Movement pathfinding │
                              │  • Action validation    │
                              │  • Coordinate transform │
                              └─────────┬───────────────┘
                                        │
                                        ▼
                              ┌─────────────────────────┐
                              │      ENGINE             │
                              │  (Command Execution)    │
                              │  • Move commands        │
                              │  • Block commands       │
                              │  • Pass commands        │
                              └─────────┬───────────────┘
                                        │
                                        ▼
                              ┌─────────────────────────┐
                              │       STATE             │
                              │  (Game State Update)    │
                              │  • PlayerTurn           │
                              │  • Setup / Kickoff      │
                              │  • Character positions  │
                              └─────────────────────────┘
```

## Responsabilités par Composant

### 🎮 Screen::handleEvent()
**Type** : Gestion UI et Navigation
**Utilisé par** : HomeScreen, TeamCreationScreen, MatchCreationScreen, EndGameScreen, GameScreen

```cpp
// RESPONSABILITÉS :
• Détecter clics sur boutons
• Gérer transitions entre écrans
• Gérer raccourcis clavier UI (Escape pour quitter, etc.)
• Filtrer/pré-traiter certains événements UI

// EXEMPLE - HomeScreen
if (buttonClicked("Nouvelle equipe")) {
    sceneManager->switchTo(TEAM_CREATION);
}
```

### 🎯 InputHandler::handleEvent()
**Type** : Logique de Gameplay
**Utilisé par** : GameScreen uniquement

```cpp
// RESPONSABILITÉS :
• Convertir coordonnées écran → plateau
• Sélectionner personnages cliquables
• Calculer chemins de mouvement valides
• Valider actions selon règles (Block légal? Move légal?)
• Créer et exécuter commandes via Engine
• Gérer états complexes (mode Move, mode Block, pending dice, etc.)

// EXEMPLE - PlayerTurn
if (leftClick) {
    auto boardPos = screenToBoard(mousePos);
    if (isPlayableCharacter(boardPos)) {
        selectCharacter(boardPos);
    } else if (selectedCharacter && isMoving) {
        addMoveStep(boardPos);
    }
}
```

## Cas d'Usage Concrets

### CAS 1 : Utilisateur clique "Nouvelle équipe" (HomeScreen)
```
Event → SceneManager → HomeScreen::handleEvent()
                       ↓
                   Détecte clic sur bouton
                       ↓
                   sceneManager->switchTo(TEAM_CREATION)
                       ↓
                   TeamCreationScreen s'affiche
```
**InputHandler** : ❌ PAS IMPLIQUÉ (c'est juste de la navigation UI)

### CAS 2 : Utilisateur sélectionne un personnage (GameScreen)
```
Event → SceneManager → GameScreen::handleEvent()
                       ↓
                   Pas un événement UI spécial
                       ↓
                   inputHandler->handleEvent()
                       ↓
                   InputHandler::handlePlayerTurnClick()
                       ↓
                   screenToBoard() → getCharacterAt()
                       ↓
                   Vérifie isPlayableCharacter()
                       ↓
                   selectedCharacter = character
                       ↓
                   currentMode = Moving
```
**Screen** : ✅ DÉLÈGUE à InputHandler

### CAS 3 : Utilisateur clique sur un dé de Block (GameScreen)
```
Event → SceneManager → GameScreen::handleEvent()
                       ↓
                   Détecte clic sur zone de dés (UI overlay)
                       ↓
                   handleDiceClick() 
                       ↓
                   inputHandler->applyPendingBlockChoice(index)
                       ↓
                   return; // Événement consommé, pas de délégation
```
**Screen ET InputHandler** : ✅ COLLABORATION (UI détecte, handler exécute)

### CAS 4 : Utilisateur déplace un personnage (GameScreen)
```
Event → SceneManager → GameScreen::handleEvent()
                       ↓
                   inputHandler->handleEvent()
                       ↓
                   InputHandler::handleRightClick()
                       ↓
                   boardPos = screenToBoard(mousePos)
                       ↓
                   Vérifie isMoveLegal(boardPos)
                       ↓
                   Créer Move command
                       ↓
                   engine->executeCommand(moveCmd)
                       ↓
                   State mis à jour
```
**InputHandler** : ✅ GÈRE TOUT (logique complexe de jeu)

### CAS 5 : Utilisateur appuie sur Escape dans le jeu (GameScreen)
```
Event → SceneManager → GameScreen::handleEvent()
                       ↓
                   Détecte Escape (après délégation à InputHandler)
                       ↓
                   sceneManager->switchTo(END_GAME)
```
**Screen** : ✅ GÈRE DIRECTEMENT (navigation globale)

## Patterns Utilisés

### 1. Chain of Responsibility
```
Event → Screen (peut consommer)
        ↓ (ou déléguer)
        InputHandler (peut consommer)
        ↓ (ou déléguer)
        Engine → State
```

### 2. Delegation Pattern
```
GameScreen délègue le gameplay à InputHandler
InputHandler délègue l'exécution à Engine
```

### 3. Strategy Pattern (implicite)
```
InputHandler adapte son comportement selon :
• currentMode (Idle, Moving, Blocking, etc.)
• game->getCurrentState() (Setup, PlayerTurn, Kickoff, etc.)
```

## Avantages de Cette Architecture

### ✅ Testabilité
```cpp
// Test InputHandler sans SFML
TEST(InputHandler, SelectCharacter) {
    auto handler = InputHandler(game, engine);
    handler.handleLeftClick({12, 6});
    EXPECT_EQ(handler.getSelectedCharacter()->getId(), 1);
}

// Test Screen sans logique de jeu
TEST(HomeScreen, ButtonClick) {
    auto screen = HomeScreen();
    // Simuler clic...
    EXPECT_EQ(sceneManager->getCurrentScene(), TEAM_CREATION);
}
```

### ✅ Séparation des Préoccupations
- **UI Designer** peut modifier les écrans sans toucher à la logique
- **Game Developer** peut modifier les règles sans toucher à l'UI
- **Input System** peut être refactorisé indépendamment

### ✅ Réutilisabilité
```cpp
// InputHandler peut être utilisé pour :
• Joueur humain (via SFML events)
• IA (génère des "clics virtuels")
• Replay system (rejoue des inputs enregistrés)
• Network (inputs reçus du réseau)
```

### ✅ Extensibilité
```cpp
// Facile d'ajouter :
• Nouveau type de screen (TutorialScreen)
• Nouveau mode d'input (TouchHandler pour mobile)
• Nouveau type d'action (Foul, HandOff, etc.)
```

## Recommandation Finale

**Votre architecture est EXCELLENTE pour un projet de cette envergure !**

Elle respecte :
- ✅ SOLID principles (Single Responsibility surtout)
- ✅ Game development best practices
- ✅ Patterns reconnus (Chain of Responsibility, Delegation)
- ✅ Séparation UI / Gameplay / Engine / State

**Continuez exactement comme ça !** 🚀

