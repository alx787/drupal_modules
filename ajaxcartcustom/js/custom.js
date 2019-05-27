(function($){
  $(document).ready(function(){
    $('.commerce-order-handler-area-order-total .component-title').html('Итого');
    // добавляем кнопки плюс-минус
    $('.view-commerce-cart-form .views-field-edit-quantity .form-item').prepend('<div class="minus"></div>');
    $('.view-commerce-cart-form .views-field-edit-quantity .form-item').append('<div class="plus"></div>');
    
    //используем live() или on() в зависимости от версии jquery
    $('.view-commerce-cart-form .minus').live('click', function(){
      //проверяем, чтобы в текстовом поле количества было число больше 1, чтобы количество не стало ниже 1
      if(parseInt($(this).parent().parent().parent().find('.views-field-edit-quantity input').val()) > 1){
        //возможно количество parent() у вас будет другое
        var count = $(this).parent().parent().parent().find('.views-field-edit-quantity input').val();
        //уменьшаем количество на 1
        count--;
        //записываем новое количество в текстовое поле
        $(this).parent().parent().parent().find('.views-field-edit-quantity input').val(count);
        //вытаскиваем из поля цены число в рублях, фильтруем, обрезаем пробелы и потом вытаскиваем число
        var price = parseFloat($.trim($(this).parent().parent().parent().find('.views-field-commerce-unit-price').html()).replace(/ /,'').replace(/,/,'.'));
        //вычисляем новое количество для строки
        var total = price * count;
        $(this).parent().parent().parent().find('.views-field-commerce-total').html(total + ',00 руб.');
        //высчитываем полную стоимость корзины
        var allTotal = 0;
        $('.view-commerce-cart-form tbody .views-field-commerce-total').each(function(){
          allTotal = allTotal + parseFloat($.trim($(this).html()).replace(/ /,'').replace(/,/,'.'));
          console.log($(this).html());
        });
        $('.view-commerce-cart-form .line-item-total-raw').html(allTotal + ',00 руб.');
        //вытаскиваем из поля line item id, поля должно быть выведено во view и скрыто 
        var lineItemId = parseInt($(this).parent().parent().parent().find('.views-field-line-item-id').html());
        //ajax запрос на изменение корзины 
        $.ajax({
          url: '/ajax/change-cart/' + count + '/' + lineItemId
        }); 
      }
    });    
    //тоже самое для плюса
    $('.view-commerce-cart-form .plus').live('click', function(){
      if(parseInt($(this).parent().parent().parent().find('.views-field-edit-quantity input').val()) > 0){
        var count = $(this).parent().parent().parent().find('.views-field-edit-quantity input').val();
        count++;
        $(this).parent().parent().parent().find('.views-field-edit-quantity input').val(count);        
        var price = parseFloat($.trim($(this).parent().parent().parent().find('.views-field-commerce-unit-price').html()).replace(/ /,'').replace(/,/,'.'));
        console.log(price);  
        var total = price * count;
        $(this).parent().parent().parent().find('.views-field-commerce-total').html(total + ',00 руб.');
        var allTotal = 0;
        $('.view-commerce-cart-form tbody .views-field-commerce-total').each(function(){
          allTotal = allTotal + parseFloat($.trim($(this).html()).replace(/ /,'').replace(/,/,'.'));
          console.log($(this).html());
        });
        $('.view-commerce-cart-form .line-item-total-raw').html(allTotal + ',00 руб.');      

        var lineItemId = parseInt($(this).parent().parent().parent().find('.views-field-line-item-id').html());

        console.log($(this).parent().parent().parent().find('.views-field-line-item-id').html());

        $.ajax({
          url: '/ajax/change-cart/' + count + '/' + lineItemId
        });         
      }
    });
  });
})(jQuery);