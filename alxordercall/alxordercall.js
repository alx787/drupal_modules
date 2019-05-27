
(function($){
    $(document).ready(function(){
        $('#callorder_onproc').click(function(){
            showDialog();
        });

        function showDialog() {
            $('<div></div>').appendTo('body')
                .html('<div><h6>Пометить как обработанное ?</h6></div>')
                .dialog({
                    modal: true,
                    title: 'Изменение статуса заявки',
                    zIndex: 10000,
                    autoOpen: true,
                    width: 'auto',
                    resizable: false,
                    buttons: {
                        Yes: function () {
                            // $(obj).removeAttr('onclick');                                
                            // $(obj).parents('.Parent').remove();

                            $.ajax({
                                type: 'GET',
                                url: $('input[name=url]').val() + 'admin/config/callbackform/setorderstatus/1/' + $('input[name=call_id]').val(),
                                success: function(msg){
                                    setTimeout(function() {window.location.reload();}, 1000);
                                    //location.reload;
                                    //alert(msg);
                                }

                            });

                            $(this).dialog("close");
                        },
                        No: function () {
                            $(this).dialog("close");
                        }
                    },
                    close: function (event, ui) {
                        $(this).remove();
                    }
                });

        }


    });
})(jQuery);

