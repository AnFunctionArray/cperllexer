
use re 'eval';

$castexprinsidebinary = qr{(?(DEFINE)$mainregexdefs)
    (?&castexpr)
}sxxo;

1