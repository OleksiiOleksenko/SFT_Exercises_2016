library(lattice)

read.table('output', header=T) -> x
x$time <- 1:length(x[,1])

xyplot(mem / 1024~time
       , group=pid
       , data=x
       , scales=list(y=list(relation="free"))
       , type = c('g','b')
       , ylab = "used memory (MB)"
       , xlab = "time (s)"
       , auto.key = T
       )
