#package cporgram;

%defaultscopevalues = {'identifiers' => [],
                        'lastscope' => []
                        };

@scope = %defaultscopevalues;

sub beginscope {
    push @scope, %defaultscopevalues;
}

sub endscope {
    print Dumper(@scope);
    pop @scope;
}

{
    1
}