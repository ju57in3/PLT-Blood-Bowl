## Proposition de FSM v2:

```mermaid
graph TD
    A[Init] --> E[PLAYER_TURN]
    E --> F[START_TURN]
    F --> G[CHOOSE_ACTION]

    G --> H[MOVE]
    H --> I[CHECK_BLOCK]
    I --> J[RESOLVE_BLOCK]
    J --> K[CHECK_INJURY]
    K --> L[CHECK_TOUCHDOWN]
    
    G --> M[PASS]
    M --> N[RESOLVE_PASS]
    N --> L
    
    G --> O[FOUL]
    O --> P[RESOLVE_FOUL]
    P --> L
    
    G --> Q[BLITZ]
    Q --> H
    
    L --> S{TOUCHDOWN?}
    S -->|Oui| T[SCORE]
    S -->|Non| U[CHECK_TURNOVER]
    
    T --> A
    
    U --> V{TURNOVER?}
    V -->|Oui| W[END_TURN]
    V -->|Non| G
    
    W --> X{HALF_TIME?}
    X -->|Oui| Y[HALF_TIME]
    X -->|Non| Z[SWITCH_TEAM]
    
    Y --> AA{GAME_END?}
    AA -->|Non| Z
    AA -->|Oui| AB[GAME_OVER]
    
    Z --> E
```

## Etat Actuel de la FSM:

```mermaid
graph TD
    Setup[Setup] --> |SetupEnded == TRUE| Kickoff[Kickoff]
    Kickoff --> PlayerTurn[Player Turn]
    PlayerTurn --> | EndTurn or TrunOver or Touchdown == TRUE| PlayerTurn
    PlayerTurn --> |TurnCounter == 16| HalfTime[Half Time]
    PlayerTurn --> |TurnCounter == 32| EndGame[EndGame]
    HalfTime --> Setup
    EndGame --> |restart == TRUE| Setup