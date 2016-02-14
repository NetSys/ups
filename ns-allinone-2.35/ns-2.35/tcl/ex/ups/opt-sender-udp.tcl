Class Application/FTP/OptSenderUDP -superclass { Application/FTP }

Application/FTP/OptSenderUDP instproc init {} {
    global opt flowcdf

    $self set npkts_ 0
    $self set laststart_ 0.0
    $self set lastack_ 0
    $self set lastrtt_ 0.0
    $self set npkts_ 0
    $self set on_duration_ 0.0
    $self next
}

Application/FTP/OptSenderUDP instproc setup_and_start { id flowid flowsize sendtime endtime } {
    $self instvar id_ flowid_ flowsize_ sendtime_  
    global ns opt
    set id_ $id
    set flowid_ $flowid
    set flowsize_ $flowsize
    set sendtime_ $sendtime
    $ns at $sendtime "$self send $flowsize_"
    $ns at $endtime "$self wrapup"
}

Application/FTP/OptSenderUDP instproc send { bytes_or_time } {
    global ns opt
    $self instvar id_ flowid_ npkts_ sentinel_ laststart_ on_duration_
    
    set nbytes [expr int($bytes_or_time)]
    set npkts_ [expr int($nbytes / $opt(pktsize))]; 
    [$self agent] send $nbytes
}


Application/FTP/OptSenderUDP instproc wrapup { } {
  [$self agent] wrapup
}
