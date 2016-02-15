Class Application/FTP/OptSender -superclass { Application/FTP }

Application/FTP/OptSender instproc init {} {
    global opt flowcdf

    $self set npkts_ 0
    $self set laststart_ 0.0
    $self set lastack_ 0
    $self set lastrtt_ 0.0
    $self set npkts_ 0
    $self set on_duration_ 0.0
    $self next
}

Application/FTP/OptSender instproc setup_and_start { id flowid tcp flowsize sendtime } {
    $self instvar id_ flowid_ tcp_ flowsize_ sendtime_ stats_ sentinel_ 
    global ns opt

    set id_ $id
    set flowid_ $flowid
    set tcp_ $tcp
    set flowsize_ $flowsize
    set sendtime_ $sendtime
    $ns at $sendtime "$self send $flowsize_"
}

Application/FTP/OptSender instproc send { bytes_or_time } {
    global ns opt
    $self instvar id_ flowid_ npkts_ sentinel_ laststart_ on_duration_
    
    set nbytes [expr int($bytes_or_time)]
    set npkts_ [expr int($nbytes / $opt(pktsize))]; # pkts for this on period
    [$self agent] send $nbytes
}


