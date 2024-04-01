-------------------------------------------------------------------------------
- Cheese version 1.2
-------------------------------------------------------------------------------

Author   : Patrice DUHAMEL (cheesechess@free.fr)

Web site : cheesechess.free.fr/index_en.html

-------------------------------------------------------------------------------
Installation
-------------------------------------------------------------------------------

* Arena Chess :

  - Unzip all files in a folder
    
  - Launch Arena Chess

  - In "Engines" Menu, choose "Install New Engine"

  - Choose UCI or Winboard (the engine works in both modes)

  - Choose Cheese executable in the folder it was installed


* Winboard

  - Unzip all files in a folder

  - to be able to choose Cheese in the winboard startup dialog :
 
  - Edit the file winboard.ini in Winboard folder

  - find the lines : /firstChessProgramNames=
               and : /sedondChessProgramNames=

  - add the name of Cheese executable in the list (without .exe)
   

* Parameters

  The file cheese.ini let you choose some parameters :

  Hash = size of the Hash table in Mb (ex: 8, 16, 32 ,64, 128, 256)

  Book = 1 or 0 , 1 = use an opening book, compatible with Cheese (cheese use his own file format)
			
  BookFile = path to the opening book file


* Opening books

  Cheese can use an opening book in his own format.

  The book cheeseopenbook.bin was created from gm2600.pgn, with a depth of
  10 half-moves, for openings used a minimum of 5 times.
  

-------------------------------------------------------------------------------
Features
-------------------------------------------------------------------------------

    * Engine compatible UCI / Windboard
    * Bitboards and magic bitboards
    * alpha-beta search (negamax)
    * Iterative deepening
    * Aspiration window
    * Quiescence search
    * Opening book (own file format)
    * Principal Variation Search
    * Internal iterative deepening
    * Adaptive null moves pruning
    * Late move reduction
    * Futility pruning + extended + razoring
    * Transposition table + Zobrist hash key
          * Use Mersenne Twister for random numbers
          * Replacement scheme : always 
    * Opening book
    * Search extensions
          * Fractional ply
          * Check extension
          * Mate threats
          * Pawn on 7th 
    * Evaluation fonction
          * Game phase (incremental)
          * Material (incremental)
          * Mobility + safe mobility
          * Piece tables
          * Penality for early queen move
          * Doubled pawns
          * Passed pawns + connected
          * Isolated pawns
          * Backward pawns
          * Blocked pawns 
          * Rooks on open or semi-open files
          * Rooks and Queen on 7th rank
          * King safety + tropism
          * Insuficiant Material
	  * Knight outposts
    * Move ordering
          * Hash table
          * Static Exchange Evaluation
          * Promotion
          * Killer moves (depth and depth-2)
          * Mate killer move
          * History heuristic 


-------------------------------------------------------------------------------
Previous versions
-------------------------------------------------------------------------------

    * Version 1.2 - May 24 2008

          * fix timing problem : in winboard opening moves was not always count
          * use magic bitboards for Bishops, Rooks, and Queens
          * optimize move generation and SEE
          * Linux version available

    * Version 1.1b - January 22 2008

          * fix problems of illegal moves with promotions
          * fix problems of illegal moves for Winboard
          * fix score output in Winboard
          * fix timing problems

    * Version 1.1 - January 2 2008
  
          * fix problem in move generation
          * fix problems with UCI and Winboard
          * fix problem with extensions
          * fix problems + optimization in SEE
          * fix problem when a non power of two size was used for hash tables
          * change the method of communication with the interface
          * change the way to count nodes
          * better time management
          * add recapture extension
          * add bonus for pawns in end game
          * add knight outposts
          * add bonus for connected passed pawns 

    * Version 1.0c - November 12 2007

          * fix problem in time management
          * fix problems in UCI and Winboard engine, in tournament mode
          * (previous versions was not working in tournament mode)
          * don't think when only one legal move
          * possibility to use opening book 
	  * fix problems when sending info to the interface during the search

    * Version 1.0b - November 4 2007

          * solve problems with Winboard protocol
          * change time function 

    * Version 1.0 - October 19 2007 (first release)

          * cleaning code
          * fix problem with FEN strings
          * fix mate value
          * add other search modes (UCI)
          * add safe mobility
          * add futility, extended futility + razoring
          * increase UCI command buffer
          * fix problem in pieces square table
          * fix problem in safe mobilitty
          * adjust mate value in hash table
          * fix problem with null moves in endgame
          * fix problem with Internal Iterative Deepening
          * add fractional ply
          * add mate threat extension
          * add pawn on 7th extension
          * optimize move generation functions 


    * Version 0.9 - May 4 2007

          * cleaning code
          * add 3-fold repetitions
          * add 50 moves rule
          * fix in move ordering
          * give higher value to passed pawns
          * add insuficiant material evaluation
          * optimize move generation
          * fix incremental material evaluation
          * add bonus for bishop pair
          * fix problem with null moves
          * fix problem with late move reduction
          * add mate killer, killers at depth-2
          * add incremental gamephase
          * fix problem in quiescence search
          * optimize move ordering
          * minimize number of call of inCheck function
          * add adaptive Null Move pruning
          * fix problem with FEN strings 


    * Version 0.8 - April 2 2007

          * fix problem in null moves
          * add Principal Variation Search
          * add incremental material evaluation
          * add gamephase in evaluation function
          * add penality for early queen move
          * add bonus and penality for pawns
          * add bonus for rooks on open / semi-open files
          * add Static Exchange Evaluation
          * add check extension
          * add bonus for rooks on 7th rank
          * add king safety and tropism
          * fix problem with UCI engine
          * add Late Move Reduction
          * add Internal Iterative Deepening 


    * Version 0.7 - January 21 2007

          * add UCI / Winboard engine
          * fix problem in iterative deepening
          * add aspiration window
          * add quiescent search
          * fix problem in alpha-beta
          * add null moves pruning
          * fix problem with history heuristic
          * optimize evaluation function
          * fix problem in move generation
          * ajout aspiration window 


    * Version 0.6 - December 19 2006

          * optimize alpha-beta search
          * add iterative deepening
          * optimize move generation
          * add move ordering, MVV/LVA and killer moves
          * add function to run testsuites in epd format
          * add history heuristic 


    * Version 0.5 - March 22 2006

          * add Zobrist hash key + Transposition Table
          * add FEN strings
          * fix and optimize move generation
          * add Perft function
          * add divide function
          * optimize hash key calculation
          * fix problem in alpha-beta 


    * Version 0.2 - February 26 2006

          * Rotated Bitboards
          * Move generation
          * check evasion function
          * Simple alpha-beta search 


    * Start - February 6 2006

         * Simple interface with SDL + OpenGL, no rules 


-------------------------------------------------------------------------------
Thanks
-------------------------------------------------------------------------------

Thanks to Grant Osborne, Robert M. Hyatt, Bruce Morland, Jonatan Peterson,
Gerd Isenberg, Pradu Kannan,
And all people on Winboard and TalkChess forums.

And thanks to all people testing or reporting problems.


-------------------------------------------------------------------------------
License
-------------------------------------------------------------------------------

Copyright 2008 DUHAMEL Patrice.
This program is freely distributed, and must not be sold or distributed
without the author authorisation.

