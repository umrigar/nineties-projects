/* Example of a grammar which is LALR(1), but not SLR(1).  
 * Taken from Tremblay & Sorenson, pg. 359.
 */

%%

G	: E '=' E
        | 'f'
        ;
E	: E '+' T
        | T
        ;
T	: 'f'
        | T '*' 'f'
        ;
